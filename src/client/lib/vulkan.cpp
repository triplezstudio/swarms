module;

#include <iostream>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

//#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan_raii.hpp>
#include <SDL_syswm.h>
#include <vulkan/vulkan_win32.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <limits>
#include <fstream>

module render.vulkan;

import common;

render::VulkanRenderer::VulkanRenderer(const render::VulkanInitData& vulkanInitData)
  : extensions(vulkanInitData.extensions), nativeHandles(vulkanInitData.nativeHandles),
  vulkanInitData(vulkanInitData)
{

}


void render::VulkanRenderer::draw()
{

}

void render::VulkanRenderer::initSurface()
{
  surface = vulkanInitData.surfaceCreationFunc.value()(instance);
}

void render::VulkanRenderer::createInstance()
{
  vk::ApplicationInfo appInfo;
  appInfo.setPApplicationName("swarms")
    .setApplicationVersion(VK_MAKE_VERSION(0, 0, 1))
    .setPEngineName("triplez")
    .setEngineVersion(VK_MAKE_VERSION(0, 0, 1))
    .setApiVersion(vk::ApiVersion13);


  auto availableExtensions = context.enumerateInstanceExtensionProperties();
  std::cout << "Available extensions:\n";
  for (const auto& ext : availableExtensions) {
    std::cout << "\t" << ext.extensionName << "\n";
  }

  enableValidationLayers();

  vk::InstanceCreateInfo createInfo = {};
  createInfo.setPApplicationInfo(&appInfo);
  createInfo.setPEnabledExtensionNames(extensions);
  createInfo.setPEnabledLayerNames(requiredLayers);

  instance = vk::raii::Instance(context, createInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);
  std::cout << "Vulkan instance created" << std::endl;
}

vk::Extent2D render::VulkanRenderer::selectSwapExtent(vk::SurfaceCapabilitiesKHR const & capabilities)
{
  if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
  {
    return capabilities.currentExtent;
  }
  int width, height;
  vulkanInitData.displaySizeFunc(&width, &height);

  return {
    std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
    std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
  };
}

vk::PresentModeKHR render::VulkanRenderer::selectSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
  assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
  return std::ranges::any_of(availablePresentModes,
                             [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
           vk::PresentModeKHR::eMailbox :
           vk::PresentModeKHR::eFifo;
}

/**
 * Number of color chanenls and types.
 * @param availableFormats All the available formats of our device.
 * @return The "best" fitting format.
 * TODO: add "target" / optimal format definition.
 * Currently we look for 32Bit BGRA SRGB color format.
 */
vk::SurfaceFormatKHR render::VulkanRenderer::selectSurfaceColorFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
{
  if (availableFormats.empty()) {
    throw std::runtime_error("Available surface formats are empty!");
  }

  const auto formatItty = std::ranges::find_if(
      availableFormats,
      [](const auto& format)
      { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; }
    );
  return formatItty != availableFormats.end() ? *formatItty : availableFormats[0];


}

void render::VulkanRenderer::createLogicalDevice()
{
  auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
  auto graphicsQueueFamilyProperty = std::ranges::find_if(queueFamilyProperties, [](auto const& qfp) {
      return (qfp.queueFlags & vk::QueueFlagBits::eGraphics) != static_cast<vk::QueueFlags>(0);
  });
  auto graphicsIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

  // We must check if the graphics queue family supports presentation.
  // "Normally" it does, but we bail out in case it does not.
  // TODO: look at "next" graphics queue family.
  if (!physicalDevice.getSurfaceSupportKHR(graphicsIndex, *surface)) {
    throw std::runtime_error("Queue Family " + std::to_string(graphicsIndex) + " does not support presentation!");
  }

  vk::DeviceQueueCreateInfo queueCreateInfo;
  queueCreateInfo.queueFamilyIndex = graphicsIndex;
  float queuePrio = 0.5;
  queueCreateInfo.setQueuePriorities(queuePrio);

  std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName};

  vk::DeviceCreateInfo deviceCreateInfo;
  deviceCreateInfo.setPNext(&deviceFeatures.get<vk::PhysicalDeviceFeatures2>())
    .setQueueCreateInfos(queueCreateInfo)
    .setPEnabledExtensionNames(requiredDeviceExtension);

  device = vk::raii::Device(physicalDevice, deviceCreateInfo);
  graphicsQueue = vk::raii::Queue( device, graphicsIndex, 0 );

}

bool render::VulkanRenderer::isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice)
{
  auto deviceProperties = physicalDevice.getProperties();
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  deviceFeatures = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();


  // We want a discrete GPU with version >= 1.3:
  if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu &&
      deviceFeatures.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
      std::ranges::any_of(queueFamilies, [](auto const & qfp) {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      })) {

    // Print the device info if we found a valid device to use:
    std::cout << "Using device: " << deviceProperties.deviceName << std::endl;
    std::cout << "Type: " << vk::to_string(deviceProperties.deviceType) << std::endl;
    std::cout << "Api version: " << VK_API_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_API_VERSION_MINOR(deviceProperties.apiVersion) << "."
              << VK_API_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;

    return true;
  }

  return false;
}

void render::VulkanRenderer::pickPhysicalDevice()
{
  auto physicalDevices = instance.enumeratePhysicalDevices();

  if (physicalDevices.empty())
  {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  for (const auto& pd : physicalDevices)
  {
    if (isDeviceSuitable(pd))
    {
      this->physicalDevice = pd;
      break;
    }
  }
}

void render::VulkanRenderer::setupDebugMessenger()
{
  if (requiredLayers.empty()) return;

  vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                                                      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
  vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(
    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
  vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT;
  debugUtilsMessengerCreateInfoEXT.setMessageSeverity(severityFlags);
  debugUtilsMessengerCreateInfoEXT.setMessageType(messageTypeFlags);
  debugUtilsMessengerCreateInfoEXT.setPfnUserCallback(&debugCallback);
  debugMessenger = instance.createDebugUtilsMessengerEXT( debugUtilsMessengerCreateInfoEXT );

}

void render::VulkanRenderer::enableValidationLayers()
{
#ifndef NDEBUG


  const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };


  requiredLayers.assign(validationLayers.begin(), validationLayers.end());
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);


  // Check if the required layers are supported by the Vulkan implementation.
  auto layerProperties = context.enumerateInstanceLayerProperties();
  if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        return std::ranges::none_of(layerProperties,
                                    [requiredLayer](auto const& layerProperty)
                                    { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
      }))
  {
    throw std::runtime_error("One or more required layers are not supported!");
  }
#endif
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL render::VulkanRenderer::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT       severity,
                                                      vk::DebugUtilsMessageTypeFlagsEXT              type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                      void *                                         pUserData)
{
  std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

  return vk::False;
}

uint32_t render::VulkanRenderer::selectSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities) {
  auto minImageCount = (std::max)(3u, surfaceCapabilities.minImageCount);
  if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
  {
    minImageCount = surfaceCapabilities.maxImageCount;
  }
  return minImageCount;
}

void render::VulkanRenderer::createSwapChain()
{
  auto presentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
  auto surfaceCaps = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
  swapExtent = selectSwapExtent(surfaceCaps);
  auto minImageCount = selectSwapMinImageCount(surfaceCaps);

  std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
  surfaceFormat                             = selectSurfaceColorFormat(availableFormats);

  vk::SwapchainCreateInfoKHR scCreateInfo;
  scCreateInfo.setSurface(*surface);
  scCreateInfo.setMinImageCount(minImageCount);
  scCreateInfo.setImageFormat(surfaceFormat.format);
  scCreateInfo.setImageColorSpace(surfaceFormat.colorSpace);
  scCreateInfo.setImageExtent(swapExtent);
  scCreateInfo.setImageArrayLayers(1);
  scCreateInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
  scCreateInfo.setImageSharingMode(vk::SharingMode::eExclusive);
  scCreateInfo.setPreTransform(surfaceCaps.currentTransform);
  scCreateInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
  scCreateInfo.setPresentMode(selectSwapPresentMode(presentModes));
  scCreateInfo.setClipped(true);
  scCreateInfo.setOldSwapchain(nullptr);

  swapChain = vk::raii::SwapchainKHR(device, scCreateInfo);
  swapChainImages = swapChain.getImages();

}

void render::VulkanRenderer::createImageViews()
{
  if (swapChainImageViews.empty() == false)
  {
    throw std::runtime_error("SwapChain Images already created!");
  }

  vk::ImageViewCreateInfo ivcr;
  ivcr.setViewType(vk::ImageViewType::e2D)
  .setFormat(surfaceFormat.format)
  .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

  for (auto & image : swapChainImages)
  {
    ivcr.image = image;
    swapChainImageViews.emplace_back(device, ivcr);
  }
}

vk::raii::ShaderModule render::VulkanRenderer::createSlangShaderModule(const std::string& shaderBinaryPath)
{
  std::vector<char> buffer;
  {
    std::ifstream file(shaderBinaryPath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
      throw std::runtime_error("failed to open file!");
    }
    buffer.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();
  }

  vk::ShaderModuleCreateInfo ci;
  ci.setCodeSize(buffer.size() * sizeof(char))
    .setPCode(reinterpret_cast<const uint32_t*>(buffer.data()));

  vk::raii::ShaderModule shaderModule(device, ci);
  return shaderModule;
}

void render::VulkanRenderer::createGraphicsPipeline()
{
  auto shaderModule = createSlangShaderModule("shader_binaries/default_shader.slang.spv");

  vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
  vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex).setModule(shaderModule).setPName("vertMain");
  vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
  fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment).setModule(shaderModule).setPName("fragMain");
  vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};


  std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dsCreateInfo;
  dsCreateInfo.setDynamicStates(dynamicStates);

  // Leaving empty for now, as we are first only using hardcoded values.
  vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
  vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
  inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
  vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
  viewportStateCreateInfo.setViewportCount(1).setScissorCount(1);

  vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
  rasterizationStateCreateInfo.setDepthClampEnable(vk::False)
  .setRasterizerDiscardEnable(vk::False)
  .setPolygonMode(vk::PolygonMode::eFill)
  .setCullMode(vk::CullModeFlagBits::eBack)
  .setFrontFace(vk::FrontFace::eClockwise)
  .setDepthBiasEnable(vk::False)
  .setLineWidth(1.0f);

  vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
  multisampleStateCreateInfo.setRasterizationSamples(vk::SampleCountFlagBits::e1)
  .setSampleShadingEnable(vk::False);

  vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
  colorBlendAttachmentState.setBlendEnable(vk::False)
  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB |
                       vk::ColorComponentFlagBits::eA);

  vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
  colorBlendStateCreateInfo.setLogicOpEnable(vk::False)
  .setLogicOp(vk::LogicOp::eCopy)
  .setPAttachments(&colorBlendAttachmentState);

  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
  pipelineLayoutCreateInfo.setSetLayoutCount(0).setPushConstantRangeCount(0);
  pipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutCreateInfo);


}

void render::VulkanRenderer::init()
{

  createInstance();
  setupDebugMessenger();
  initSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createGraphicsPipeline();
}

