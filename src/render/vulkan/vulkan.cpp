#include <iostream>
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include <vulkan/vulkan_raii.hpp>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#ifdef _WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif

#include <vulkan_renderer.hh>

#include <algorithm>
#include <limits>
#include <fstream>

tz::WindowDesc tz::render::vulkan::VulkanRenderer::getRequiredWindowDesc()
{
  WindowDesc wd;
  wd.api = tz::WindowDesc::GraphicsAPI::Vulkan;
  wd.width = 1280;
  wd.height = 720;
  return wd;
}

void tz::render::vulkan::VulkanRenderer::clearScreen()
{

}

void tz::render::vulkan::VulkanRenderer::beginDraw(PrimitiveType primitiveType)
{

}

void tz::render::vulkan::VulkanRenderer::endDraw()
{

}

void tz::render::vulkan::VulkanRenderer::emitPosition(Eigen::Vector3f position)
{

}
void tz::render::vulkan::VulkanRenderer::emitColor(Eigen::Vector4f color)
{

}
void tz::render::vulkan::VulkanRenderer::emitUV(Eigen::Vector2f uv)
{

}
void tz::render::vulkan::VulkanRenderer::emitNormal(Eigen::Vector3f normal)
{

}

void tz::render::vulkan::VulkanRenderer::beginFrame()
{

}
void tz::render::vulkan::VulkanRenderer::endFrame()
{

}
void tz::render::vulkan::VulkanRenderer::submitCommandBuffer(CommandBuffer* commandBuffer)
{

}

void tz::render::vulkan::VulkanRenderer::initSurface()
{
  GraphicsInstance gi;
  gi.handle = reinterpret_cast<void*>(static_cast<VkInstance>(*instance));
  auto rawSurface = window->surfaceCreationFunc(gi, getRequiredWindowDesc());
  auto surfKHR = reinterpret_cast<VkSurfaceKHR>(rawSurface.handle);
  surface = vk::raii::SurfaceKHR(instance, surfKHR);
}

void tz::render::vulkan::VulkanRenderer::createInstance()
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
    std::cout << "\t" << ext.extensionName << std::endl;
    extensions.push_back(ext.extensionName);
  }

  enableValidationLayers();

  vk::InstanceCreateInfo createInfo = {};
  createInfo.setPApplicationInfo(&appInfo);
  createInfo.setPEnabledExtensionNames(extensions);
  createInfo.setPEnabledLayerNames(requiredLayers);

  VULKAN_HPP_DEFAULT_DISPATCHER.init(
    reinterpret_cast<PFN_vkGetInstanceProcAddr>(vkGetInstanceProcAddr));
  instance = vk::raii::Instance(context, createInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);
  std::cout << "Vulkan instance created" << std::endl;
}

vk::Extent2D tz::render::vulkan::VulkanRenderer::selectSwapExtent(vk::SurfaceCapabilitiesKHR const & capabilities)
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

vk::PresentModeKHR tz::render::vulkan::VulkanRenderer::selectSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
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
vk::SurfaceFormatKHR tz::render::vulkan::VulkanRenderer::selectSurfaceColorFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats)
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

void tz::render::vulkan::VulkanRenderer::createLogicalDevice()
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
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
  graphicsQueue = vk::raii::Queue( device, graphicsIndex, 0 );

}

bool tz::render::vulkan::VulkanRenderer::isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice)
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

void tz::render::vulkan::VulkanRenderer::pickPhysicalDevice()
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

void tz::render::vulkan::VulkanRenderer::setupDebugMessenger()
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

void tz::render::vulkan::VulkanRenderer::enableValidationLayers()
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

VKAPI_ATTR vk::Bool32 VKAPI_CALL tz::render::vulkan::VulkanRenderer::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT       severity,
                                                      vk::DebugUtilsMessageTypeFlagsEXT              type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                      void *                                         pUserData)
{
  std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;

  return vk::False;
}

uint32_t tz::render::vulkan::VulkanRenderer::selectSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities) {
  auto minImageCount = (std::max)(3u, surfaceCapabilities.minImageCount);
  if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
  {
    minImageCount = surfaceCapabilities.maxImageCount;
  }
  return minImageCount;
}

void tz::render::vulkan::VulkanRenderer::createSwapChain()
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

void tz::render::vulkan::VulkanRenderer::createImageViews()
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

vk::raii::ShaderModule tz::render::vulkan::VulkanRenderer::createSlangShaderModule(const std::string& shaderBinaryPath)
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

void tz::render::vulkan::VulkanRenderer::createGraphicsPipeline()
{
  auto shaderModule = createSlangShaderModule("shader_binaries/default_shader.slang.spv");

  vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
  vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex).setModule(shaderModule).setPName("main");
  vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
  fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment).setModule(shaderModule).setPName("main");
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

  vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
  pipelineRenderingCreateInfo.setColorAttachmentCount(1);
  pipelineRenderingCreateInfo.setPColorAttachmentFormats(&surfaceFormat.format);

  vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain;
  pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    .setStages(shaderStages)
    .setPVertexInputState(&vertexInputStateCreateInfo)
    .setPInputAssemblyState(&inputAssembly)
    .setPRasterizationState(&rasterizationStateCreateInfo)
    .setPViewportState(&viewportStateCreateInfo)
    .setPMultisampleState(&multisampleStateCreateInfo)
    .setPColorBlendState(&colorBlendStateCreateInfo)
    .setPDynamicState(&dsCreateInfo)
    .setLayout(pipelineLayout)
    .setRenderPass(nullptr);

  // TODO is this equvivalent with the one below?
  //pipelineCreateInfoChain.assign(pipelineRenderingCreateInfo);

  pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>()
    .setPColorAttachmentFormats(&surfaceFormat.format);

  graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());



}

void tz::render::vulkan::VulkanRenderer::init(tz::Window* window)
{
  this->window = window;
  createInstance();
  setupDebugMessenger();
  initSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
  createImageViews();
  createGraphicsPipeline();
}



tz::Buffer* tz::render::vulkan::VulkanRenderer::createBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage)
{
  vk::BufferCreateInfo createInfo;
  createInfo.setSize(sizeInBytes);
  createInfo.setUsage(toVkBufferUsage(bufferUsage));
  createInfo.setSharingMode(vk::SharingMode::eExclusive);
  auto b = vk::raii::Buffer(device, createInfo);
  return new VulkanBuffer(std::move(b));
}

