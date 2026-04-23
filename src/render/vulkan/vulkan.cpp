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
  currentFrameIndex    = (currentFrameIndex + 1) % maxFramesInFlight;
  auto fenceResult = device.waitForFences(*drawFences[currentFrameIndex], vk::True, UINT64_MAX);
  if (fenceResult != vk::Result::eSuccess)
  {
    throw std::runtime_error("failed to wait for fence!");
  }
  device.resetFences(*drawFences[currentFrameIndex]);

  // Use the semaphore indexed by the currently held image index (from previous frame)
  // This avoids reusing a semaphore that might still be in use by the swapchain
  auto [result, nextImageIndex] = swapChain.acquireNextImage(UINT64_MAX, *presentCompleteSemaphores[currentFrameIndex], nullptr);

  imageIndex = nextImageIndex;
}

void tz::render::vulkan::VulkanRenderer::endFrame()
{

}
void tz::render::vulkan::VulkanRenderer::submitCommandBuffer(CommandBuffer* cb)
{
  auto& currentFrameCommandBuffer = getCommandBufferForCurrentFrame(cb);
  vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  const vk::SubmitInfo submitInfo {.waitSemaphoreCount = 1,
                                    .pWaitSemaphores = &*presentCompleteSemaphores[currentFrameIndex],
                                    .pWaitDstStageMask = &waitDestinationStageMask,
                                    .commandBufferCount = 1,
                                    .pCommandBuffers = &*currentFrameCommandBuffer,
                                    .signalSemaphoreCount = 1,
                                    .pSignalSemaphores = &*renderFinishedSemaphores[currentFrameIndex]};

  graphicsQueue.submit(submitInfo, *drawFences[currentFrameIndex]);

  const vk::PresentInfoKHR presentInfoKhr {
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &*renderFinishedSemaphores[currentFrameIndex],
    .swapchainCount = 1,
    .pSwapchains = &*swapChain,
    .pImageIndices = &imageIndex};

  auto result = graphicsQueue.presentKHR(presentInfoKhr);


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
  window->displaySizeFunc(&width, &height);

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
  graphicsQueueIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));

  // We must check if the graphics queue family supports presentation.
  // "Normally" it does, but we bail out in case it does not.
  // TODO: look at "next" graphics queue family.
  if (!physicalDevice.getSurfaceSupportKHR(graphicsQueueIndex, *surface)) {
    throw std::runtime_error("Queue Family " + std::to_string(graphicsQueueIndex) + " does not support presentation!");
  }

  vk::DeviceQueueCreateInfo queueCreateInfo;
  queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
  float queuePrio = 0.5;
  queueCreateInfo.setQueuePriorities(queuePrio);

  std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName,
    vk::KHRShaderDrawParametersExtensionName};

  vk::DeviceCreateInfo deviceCreateInfo;
  deviceCreateInfo.setPNext(&deviceFeatures.get<vk::PhysicalDeviceFeatures2>())
    .setQueueCreateInfos(queueCreateInfo)
    .setPEnabledExtensionNames(requiredDeviceExtension);

  device = vk::raii::Device(physicalDevice, deviceCreateInfo);
  VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);
  graphicsQueue = vk::raii::Queue( device, graphicsQueueIndex, 0 );

}

bool tz::render::vulkan::VulkanRenderer::isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice)
{
  auto deviceProperties = physicalDevice.getProperties();
  auto queueFamilies = physicalDevice.getQueueFamilyProperties();
  deviceFeatures = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT, vk::PhysicalDeviceShaderDrawParameterFeatures>();


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
  auto minImageCount = (std::max)(maxFramesInFlight, surfaceCapabilities.minImageCount);
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

vk::raii::ImageView tz::render::vulkan::VulkanRenderer::createVulkanImageView(vk::raii::Image &image)
{

vk::ImageViewCreateInfo viewInfo{ .image = image, .viewType = vk::ImageViewType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb, .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };
    return vk::raii::ImageView( device, viewInfo );

}

vk::raii::ShaderModule tz::render::vulkan::VulkanRenderer::createSlangShaderModule(const std::string& shaderBinaryPath)
{
  std::vector<char> buffer;
  {
    std::ifstream file(shaderBinaryPath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
      throw std::runtime_error("failed to open file " + shaderBinaryPath);
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
  .setAttachmentCount(1)
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
  pipelineCreateInfoChain.assign(pipelineRenderingCreateInfo);

  pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>()
    .setPColorAttachmentFormats(&surfaceFormat.format);

  graphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());



}

void tz::render::vulkan::VulkanRenderer::createCommandPool()
{
  vk::CommandPoolCreateInfo poolInfo;
  poolInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
  .setQueueFamilyIndex(graphicsQueueIndex);

  commandPool = vk::raii::CommandPool(device, poolInfo);

}

void tz::render::vulkan::VulkanRenderer::createDefaultCommandBuffer()
{
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(commandPool)
  .setLevel(vk::CommandBufferLevel::ePrimary)
  .setCommandBufferCount(1);
  commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());
}

/**
 * This method specifically transfers images from one layout to another.
 * @param image
 * @param oldLayout
 * @param newLayout
 */
void tz::render::vulkan::VulkanRenderer::transitionImageLayout(const vk::raii::Image& image,
                                                               vk::ImageLayout oldLayout,
                                                               vk::ImageLayout newLayout)
{
  auto cb = beginOneTimeCommandbuffer();
  vk::ImageMemoryBarrier barrier;
  barrier.oldLayout        = oldLayout;
  barrier.newLayout        = newLayout;
  barrier.image            = image;
  barrier.subresourceRange = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};
  // For defining the source and destination stage during the barrier operation,
  // we check for some specific situations.
  vk::PipelineStageFlags sourceStage;
  vk::PipelineStageFlags destinationStage;

  if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
  {
    barrier.srcAccessMask = {};
    barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
    sourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
    destinationStage      = vk::PipelineStageFlagBits::eTransfer;
  }
  else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
           && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
  {
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
    sourceStage           = vk::PipelineStageFlagBits::eTransfer;
    destinationStage      = vk::PipelineStageFlagBits::eFragmentShader;
  }
  else
  {
    throw std::invalid_argument("unsupported image layout transition!");
  }

  cb.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
  endOneTimeCommandBuffer(cb);
}

void tz::render::vulkan::VulkanRenderer::copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height)
{
  auto cb = beginOneTimeCommandbuffer();
  vk::BufferImageCopy region;
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1};
  region.imageOffset = vk::Offset3D {0, 0, 0};
  region.imageExtent = vk::Extent3D {width, height, 1};
  cb.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {region});
  endOneTimeCommandBuffer(cb);
}

void tz::render::vulkan::VulkanRenderer::transitionImageLayout(
  tz::CommandBuffer* cb,
  vk::ImageLayout oldLayout,
  vk::ImageLayout newLayout,
  vk::AccessFlags2 srcAccessMask,
  vk::AccessFlags2 dstAccessMask,
  vk::PipelineStageFlags2 srcStageMask,
  vk::PipelineStageFlags2 dstStageMask)
{
  auto& currentFrameCommandBuffer = getCommandBufferForCurrentFrame(cb);

  vk::ImageMemoryBarrier2 barrier;
  barrier.setSrcStageMask(srcStageMask)
    .setDstStageMask(dstStageMask)
    .setSrcAccessMask(srcAccessMask)
    .setDstAccessMask(dstAccessMask)
    .setOldLayout(oldLayout)
    .setNewLayout(newLayout)
    .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
    .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
    .setImage(swapChainImages[imageIndex])
    .setSubresourceRange({
      .aspectMask = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
    });
  vk::DependencyInfo dependencyInfo;
  dependencyInfo.setDependencyFlags({})
    .setImageMemoryBarriers({barrier});

  currentFrameCommandBuffer.pipelineBarrier2(dependencyInfo);
}

void tz::render::vulkan::VulkanRenderer::transitionImageLayout(
  uint32_t imageIndex,
  vk::ImageLayout oldLayout,
  vk::ImageLayout newLayout,
  vk::AccessFlags2 srcAccessMask,
  vk::AccessFlags2 dstAccessMask,
  vk::PipelineStageFlags2 srcStageMask,
  vk::PipelineStageFlags2 dstStageMask)
{
  vk::ImageMemoryBarrier2 barrier;
  barrier.setSrcStageMask(srcStageMask)
  .setDstStageMask(dstStageMask)
  .setSrcAccessMask(srcAccessMask)
  .setDstAccessMask(dstAccessMask)
  .setOldLayout(oldLayout)
  .setNewLayout(newLayout)
  .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
  .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
  .setImage(swapChainImages[imageIndex])
  .setSubresourceRange({
      .aspectMask = vk::ImageAspectFlagBits::eColor,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1
    });
  vk::DependencyInfo dependencyInfo;
  dependencyInfo.setDependencyFlags({})
    .setImageMemoryBarriers({barrier});

  commandBuffer.pipelineBarrier2(dependencyInfo);


}

/**
 * This function is mainly a PoC and just hardcodes some commands.
 * TOTO probably remove after initial implementation stages!?
 * @param imageIndex    The current "frame-in-flight"
 */
void tz::render::vulkan::VulkanRenderer::recordDefaultCommandBuffer()
{

  vk::CommandBufferBeginInfo beginInfo;
  // Currently we do not use the beginInfo:
  commandBuffer.begin({});

  transitionImageLayout(currentFrameIndex,
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::eColorAttachmentOptimal,
    {}, // no need to wait for the src access part
    vk::AccessFlagBits2::eColorAttachmentWrite,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput

    );

  vk::ClearValue clearColor = vk::ClearColorValue(0, 0, 0, 1);
  vk::RenderingAttachmentInfo attachmentInfo;
  attachmentInfo.setImageView(swapChainImageViews[imageIndex])
  .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
  .setLoadOp(vk::AttachmentLoadOp::eClear)
  .setStoreOp(vk::AttachmentStoreOp::eStore)
  .setClearValue(clearColor);

  vk::RenderingInfo renderingInfo;
  renderingInfo.setRenderArea({
    .offset = {0, 0},
    .extent = swapExtent
  })
  .setLayerCount(1)
  .setColorAttachments({attachmentInfo});
  commandBuffer.beginRendering(renderingInfo);
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
  commandBuffer.setViewport(0, vk::Viewport(0, 0,
                                            static_cast<float>(swapExtent.width)
                                                , static_cast<float>(swapExtent.height), 0, 1));
  commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0,0), swapExtent));
  commandBuffer.draw(3, 1, 0, 0);
  commandBuffer.endRendering();
  transitionImageLayout(currentFrameIndex,
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::ImageLayout::ePresentSrcKHR,
    vk::AccessFlagBits2::eColorAttachmentWrite,
    {},
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eBottomOfPipe
    );
  commandBuffer.end();
}

void tz::render::vulkan::VulkanRenderer::createSyncObjects()
{
  // Create semaphores per swapchain image to avoid reuse issues
  for (size_t i = 0; i < swapChainImages.size(); i++)
  {
    presentCompleteSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
    renderFinishedSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
  }

  // Create fences per frame for CPU-GPU synchronization
  for (size_t i = 0; i < maxFramesInFlight; i++)
  {
    drawFences.emplace_back(device, vk::FenceCreateInfo{.flags = vk::FenceCreateFlagBits::eSignaled});

  }

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
  createCommandPool();
  createDescriptorPool();
  createDefaultCommandBuffer();
  createSyncObjects();
}

uint32_t tz::render::vulkan::VulkanRenderer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties )
{

  auto memProperties = physicalDevice.getMemoryProperties();

  // Checking each bit of the memory type bitmask and for support of cpu uploading:
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
  {
    if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
    {
      return i;
    }
  }


  throw std::runtime_error("failed to find suitable memory type!");
}

tz::Buffer* tz::render::vulkan::VulkanRenderer::createBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage)
{
  vk::BufferCreateInfo createInfo;
  createInfo.setSize(sizeInBytes);
  createInfo.setUsage(toVkBufferUsage(bufferUsage));
  createInfo.setSharingMode(vk::SharingMode::eExclusive);
  auto b = vk::raii::Buffer(device, createInfo);

  // Allocating memory for the buffer and binding it to the buffer:
  vk::MemoryRequirements memRequirements = b.getMemoryRequirements();
  vk::MemoryAllocateInfo memoryAllocateInfo;
  memoryAllocateInfo.allocationSize = memRequirements.size;
  memoryAllocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                      vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
  vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(device,memoryAllocateInfo);
  b.bindMemory(*bufferMemory, 0);

  // Filling the data into the memory:
  if (initialData)
  {
    void* targetDataPtr = bufferMemory.mapMemory(0, memRequirements.size);
    memcpy(targetDataPtr, initialData, sizeInBytes);
    bufferMemory.unmapMemory();
  }


  return new VulkanBuffer(std::move(b), std::move(bufferMemory));
}

tz::ShaderPipeline *tz::render::vulkan::VulkanRenderer::createShaderPipeline(const std::vector<ShaderModule *> &modules)
{
  auto sp = new tz::render::vulkan::VulkanShaderPipeline();
  sp->link(modules);
  return sp;
}


tz::ShaderModule *tz::render::vulkan::VulkanRenderer::createShaderModule(tz::ShaderType type,
                                                                         const std::string &sprvFilePath)
{
    auto shaderModule = createSlangShaderModule(sprvFilePath);
    vk::PipelineShaderStageCreateInfo shaderStageInfo;
    vk::ShaderStageFlagBits flags;
    switch (type)
    {
      case ShaderType::Vertex: flags = vk::ShaderStageFlagBits::eVertex; break;
      case ShaderType::Fragment: flags = vk::ShaderStageFlagBits::eFragment; break;

    }
    shaderStageInfo.setStage(flags)
      .setModule(shaderModule).setPName("main");

    auto sm = new tz::render::vulkan::VulkanShaderModule(shaderStageInfo, std::move(shaderModule));
    return reinterpret_cast<tz::ShaderModule*>(sm);

}


/**
 * This creates a commandBuffer for each frame-in-flight.
 */
tz::CommandBuffer *tz::render::vulkan::VulkanRenderer::createCommandBuffer()
{
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.setCommandPool(commandPool)
    .setLevel(vk::CommandBufferLevel::ePrimary)
    .setCommandBufferCount(maxFramesInFlight);
  auto cbs = vk::raii::CommandBuffers(device, allocInfo);
  auto size = cbs.size();

  // Store this "logical commandbuffer" to point to the actual
  auto vulkanCB = new VulkanCommandBuffer(std::move(cbs));
  return vulkanCB;

}

vk::raii::CommandBuffer& tz::render::vulkan::VulkanRenderer::getCommandBufferForCurrentFrame(tz::CommandBuffer* cb)
{
  auto& cfb = dynamic_cast<VulkanCommandBuffer*>(cb)->getCommandBufferForImage(currentFrameIndex);
  return cfb;
}


/**
 * Marks the start of the recording process of the command buffer.
 * The passed in logical commandbuffer internally holds as many "real"
 * vulkan command buffers as we have "framesInFlight".
 * So for example 2 for double buffering, 3 for triple buffering and so on.
 * First thing is always to grab the actual current vulkan commandbuffer to
 * render into.
 *
 * We are also currently implicitely clearing the main swapchain "framebuffer" now.
 *
 * @param cb    The "logical" API commandbuffer object. Wrapper for the real vulkan command buffers.
 */
void tz::render::vulkan::VulkanRenderer::beginCommandBuffer(tz::CommandBuffer *cb)
{

  auto& currentFrameCommandBuffer = getCommandBufferForCurrentFrame(cb);
  currentFrameCommandBuffer.reset();
  currentFrameCommandBuffer.begin({});

  transitionImageLayout(
    cb,
    vk::ImageLayout::eUndefined,
    vk::ImageLayout::eColorAttachmentOptimal,
    {}, // no need to wait for the src access part
    vk::AccessFlagBits2::eColorAttachmentWrite,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput
  );

  vk::ClearValue clearColor = vk::ClearColorValue(0, 0, 0, 1);
  vk::RenderingAttachmentInfo attachmentInfo;
  attachmentInfo.setImageView(swapChainImageViews[imageIndex])
    .setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
    .setLoadOp(vk::AttachmentLoadOp::eClear)
    .setStoreOp(vk::AttachmentStoreOp::eStore)
    .setClearValue(clearColor);

  vk::RenderingInfo renderingInfo;
  renderingInfo.setRenderArea({
                                .offset = {0, 0},
                                .extent = swapExtent
                              })
    .setLayerCount(1)
    .setColorAttachments({attachmentInfo});
  currentFrameCommandBuffer.beginRendering(renderingInfo);

}


void tz::render::vulkan::VulkanRenderer::recordCommand(tz::CommandBuffer* cb, tz::Command *cmd)
{

  auto& currentFrameCommandBuffer = getCommandBufferForCurrentFrame(cb);
  if (auto c = dynamic_cast<CmdBindPipeline*>(cmd))
  {
    auto vpso = reinterpret_cast<tz::render::vulkan::VulkanPSO*>(c->pso);
    currentFrameCommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *vpso->getHandle());

  }

  else if (auto c = dynamic_cast<CmdBindDescriptors*>(cmd))
  {
    // TODO: this command must have a reference to the current pipeline layout
    auto& pipelineLayout = dynamic_cast<VulkanPSO*>(c->pso)->getPipelineLayout();
    std::vector<vk::DescriptorSet> descriptorSets;
    for (auto& ds : c->descriptorSets)
    {
      auto vds = reinterpret_cast<VulkanDescriptorSet*>(ds);
      auto& vulkanDS = vds->descSets[currentFrameIndex];
      descriptorSets.push_back(*vulkanDS);
    }

    currentFrameCommandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipelineLayout,
                                                 0, descriptorSets,
                                                 nullptr);
  }

  else if (auto c = dynamic_cast<CmdBindIndexBuffer*>(cmd))
  {
    auto vib = reinterpret_cast<tz::render::vulkan::VulkanBuffer*>(c->indexBuffer);
    auto indexBuffer = vib->getBuffer();
    currentFrameCommandBuffer.bindIndexBuffer(indexBuffer, c->offset, vk::IndexType::eUint32);
  }

  else if (auto c = dynamic_cast<CmdBindVertexBuffers*>(cmd))
  {
    std::vector<vk::Buffer> rawBuffers;
    std::vector<vk::DeviceSize> offsets;
    for (auto& b : c->vertexBuffers)
    {
      auto vb = reinterpret_cast<tz::render::vulkan::VulkanBuffer*>(b);
      auto handle = vb->getBuffer();
      rawBuffers.push_back(handle);
      offsets.push_back(0);
    }
    currentFrameCommandBuffer.bindVertexBuffers(0, rawBuffers, offsets);
  }

  else if (auto c = dynamic_cast<CmdDraw*>(cmd))
  {
    currentFrameCommandBuffer.draw(static_cast<uint32_t>(c->vertexCount), c->instanceCount, c->firstVertex, c->firstInstance);
  }
  else if (auto c = dynamic_cast<CmdDrawIndexed*>(cmd))
  {
    currentFrameCommandBuffer.drawIndexed(c->indexCount, c->instanceCount, c->firstIndex, c->vertexOffset, c->firstInstance);
  }

  else if (auto c = dynamic_cast<CmdSetViewPorts*>(cmd))
  {
    // The number of viewports must actually be consistent with the
    // pipeline setup. So setting 1 there, but passing in > 1 would not work.
    std::vector<vk::Viewport> targetVps;
    for (auto& vp: c->viewPorts)
    {
      vk::Viewport targetVp;
      targetVp.width = vp.width;
      targetVp.height = -(float)vp.height;    // This flips the y coordinate, a vulkan quirk
      targetVp.x = vp.x;
      targetVp.y = vp.height;                 // Start at the botto, so we don't need any projection -y trick
      targetVp.minDepth = 0;
      targetVp.maxDepth = 1;
      targetVps.push_back(targetVp);
    }
    currentFrameCommandBuffer.setViewport(0, targetVps);
  }
  else if (auto c = dynamic_cast<CmdSetScissors*>(cmd))
  {
    std::vector<vk::Rect2D> rects;
    for (auto& s: c->scissors)
    {
      vk::Rect2D r;
      r.offset.x = s.x;
      r.offset.y = s.y;
      r.extent.width = s.width;
      r.extent.height = s.height;
      rects.push_back(r);
    }
    currentFrameCommandBuffer.setScissor(0, rects);
  }
}

vk::VertexInputRate tz::render::vulkan::VulkanRenderer::toVulkanInputRate(tz::VertexInputRate ir)
{
  switch (ir)
  {
    case tz::VertexInputRate::PerVertex: return vk::VertexInputRate::eVertex;
    case tz::VertexInputRate::PerInstance: return vk::VertexInputRate::eInstance;
  }
}

vk::Format tz::render::vulkan::VulkanRenderer::toVulkanAttributeFormat(tz::VertexAttribute va)
{
  switch (va.componentCount)
  {
    case 1: return vk::Format::eR32Sfloat;
    case 2: return vk::Format::eR32G32Sfloat;
    case 3: return vk::Format::eR32G32B32Sfloat;
    case 4: return vk::Format::eR32G32B32A32Sfloat;
  }
}

std::vector<vk::VertexInputAttributeDescription>
  tz::render::vulkan::VulkanRenderer::toVulkanAttributeDescriptions(const std::vector<tz::VertexAttribute>& vertexAttributes)
{
  std::vector<vk::VertexInputAttributeDescription> attributeDescriptions;
  for (auto& va : vertexAttributes)
  {
    vk::VertexInputAttributeDescription viad;
    viad.binding = va.bufferSlot;
    viad.offset = va.offset;
    viad.location = va.shaderLocation;
    viad.format = toVulkanAttributeFormat(va);
    attributeDescriptions.push_back(viad);
  }

  return attributeDescriptions;
}

std::vector<vk::VertexInputBindingDescription>
  tz::render::vulkan::VulkanRenderer::toVulkanBindingDescriptions(const std::vector<tz::VertexBinding>& vertexBindings)
{
  std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions;
  for (auto& vb : vertexBindings)
  {
    vk::VertexInputBindingDescription vbd;
    vbd.binding = vb.bufferSlot;
    vbd.stride = vb.stride;
    vbd.inputRate = toVulkanInputRate(vb.vertexInputRate);
    vertexInputBindingDescriptions.push_back(vbd);
  }

  return vertexInputBindingDescriptions;

}

// TODO : WIP - implement the creation of the pipeline based on the actual incoming parameters,
// currently the implementation is based on the default graphics pipeline creation.
tz::PipelineStateObject *tz::render::vulkan::VulkanRenderer::createPipelineStateObject(
  tz::RenderState &renderState,
  tz::ShaderPipeline *shaderPipeline,
  tz::VertexLayout &vertexLayout,
  const std::vector<DescriptorSetLayout*> &descriptorSetLayouts)
{
  auto vsp = reinterpret_cast<tz::render::vulkan::VulkanShaderPipeline*>(shaderPipeline);
  auto shaderModules = *reinterpret_cast<std::vector<ShaderModule*>*>(vsp->getHandle());
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (auto& sm : shaderModules)
  {
    auto ci = reinterpret_cast<vk::PipelineShaderStageCreateInfo*>(sm->getHandle());
    shaderStages.push_back(*ci);
  }

  std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
  vk::PipelineDynamicStateCreateInfo dsCreateInfo;
  dsCreateInfo.setDynamicStates(dynamicStates);

  auto bindingDescriptions = toVulkanBindingDescriptions(vertexLayout.bindings);
  auto attributeDescriptions = toVulkanAttributeDescriptions(vertexLayout.attributes);
  vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
  vertexInputStateCreateInfo.setVertexBindingDescriptions(bindingDescriptions);
  vertexInputStateCreateInfo.setVertexAttributeDescriptions(attributeDescriptions);
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
    .setAttachmentCount(1)
    .setPAttachments(&colorBlendAttachmentState);

  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;

  std::vector<vk::DescriptorSetLayout> descSetLayouts;
  for (auto& descLayoutWrapper : descriptorSetLayouts)
  {
    auto& descLayout = reinterpret_cast<VulkanDescriptorSetLayout*>(descLayoutWrapper)->dsLayout;
    descSetLayouts.push_back(descLayout);
  }
  pipelineLayoutCreateInfo.setSetLayoutCount(descriptorSetLayouts.size());
  pipelineLayoutCreateInfo.setSetLayouts(descSetLayouts);
  pipelineLayoutCreateInfo.setPushConstantRangeCount(0);
  auto customPipelineLayout = vk::raii::PipelineLayout(device, pipelineLayoutCreateInfo);

  vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
  pipelineRenderingCreateInfo.setColorAttachmentCount(1);
  pipelineRenderingCreateInfo.setPColorAttachmentFormats(&surfaceFormat.format);

  vk::StructureChain<vk::GraphicsPipelineCreateInfo, vk::PipelineRenderingCreateInfo> pipelineCreateInfoChain;
  pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>()
    .setPStages(shaderStages.data())
    .setStageCount(shaderStages.size())
    .setPVertexInputState(&vertexInputStateCreateInfo)
    .setPInputAssemblyState(&inputAssembly)
    .setPRasterizationState(&rasterizationStateCreateInfo)
    .setPViewportState(&viewportStateCreateInfo)
    .setPMultisampleState(&multisampleStateCreateInfo)
    .setPColorBlendState(&colorBlendStateCreateInfo)
    .setPDynamicState(&dsCreateInfo)
    .setLayout(customPipelineLayout)
    .setRenderPass(nullptr);

    pipelineCreateInfoChain.assign(pipelineRenderingCreateInfo);

  pipelineCreateInfoChain.get<vk::PipelineRenderingCreateInfo>()
    .setPColorAttachmentFormats(&surfaceFormat.format);

  auto customGraphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfoChain.get<vk::GraphicsPipelineCreateInfo>());
  auto vulkanPSO = new tz::render::vulkan::VulkanPSO(std::move(customGraphicsPipeline), std::move(customPipelineLayout));
  return vulkanPSO;

}
void tz::render::vulkan::VulkanRenderer::endCommandBuffer(tz::CommandBuffer *cb)
{
  auto& currentFrameCommandBuffer = getCommandBufferForCurrentFrame(cb);
  currentFrameCommandBuffer.endRendering();
  transitionImageLayout(
    cb,
    vk::ImageLayout::eColorAttachmentOptimal,
    vk::ImageLayout::ePresentSrcKHR,
    vk::AccessFlagBits2::eColorAttachmentWrite,
    vk::AccessFlagBits2::eNone,
    vk::PipelineStageFlagBits2::eColorAttachmentOutput,
    vk::PipelineStageFlagBits2::eBottomOfPipe
  );
  currentFrameCommandBuffer.end();
}

vk::DescriptorType tz::render::vulkan::toVulkanDescriptorType(tz::ResourceType resourceType)
{
  switch (resourceType)
  {
    case tz::ResourceType::Ubo: return vk::DescriptorType::eUniformBuffer;
    case tz::ResourceType::Ssbo: return vk::DescriptorType::eStorageBuffer;
    case tz::ResourceType::Sampler: return vk::DescriptorType::eCombinedImageSampler;
  }
}

vk::ShaderStageFlagBits tz::render::vulkan::toShaderStageFlags(tz::ShaderType shaderType)
{
  switch (shaderType)
  {
    case tz::ShaderType::Vertex: return vk::ShaderStageFlagBits::eVertex;
    case tz::ShaderType::Fragment: return vk::ShaderStageFlagBits::eFragment;
    case tz::ShaderType::Tessellation: return vk::ShaderStageFlagBits::eTessellationControl;
  }
}

tz::DescriptorBinding *tz::render::vulkan::VulkanRenderer::createDescriptorBinding(
  uint8_t binding,
  tz::ResourceType resourceType,
  tz::ShaderType shaderType,
  uint32_t count,
  Buffer* buffer,
  Texture* texture
  )
{

    vk::DescriptorSetLayoutBinding layoutBinding(binding,
                                               toVulkanDescriptorType(resourceType),
                                               count,
                                               toShaderStageFlags(shaderType),
                                               nullptr);
    auto vulkanBuffer = dynamic_cast<VulkanBuffer*>(buffer);
    auto descriptorBinding = new VulkanDescriptorBinding(std::move(layoutBinding));
    descriptorBinding->buffer = buffer;
    descriptorBinding->texture = texture;
    descriptorBinding->type = resourceType;
    descriptorBinding->shaderType = shaderType;
    descriptorBinding->count = count;
    descriptorBinding->bindingIndex = binding;
    return descriptorBinding;
}



tz::Buffer *tz::render::vulkan::VulkanRenderer::createMultiframeBuffer(void *initialData,
                                                                       size_t sizeInBytes,
                                                                       tz::BufferUsage bufferUsage)
{
  std::vector<vk::raii::Buffer> vulkanBuffers;
  std::vector<vk::raii::DeviceMemory> memories;
  for (size_t i = 0; i < maxFramesInFlight; i++)
  {
    auto b = dynamic_cast<VulkanBuffer*>(createBuffer(initialData, sizeInBytes, bufferUsage));
    auto vulkanBuffer = vk::raii::Buffer(device, b->getBuffer());
    auto vulkanMemory = vk::raii::DeviceMemory(device, b->getMemoryHandle());
    vulkanBuffers.push_back(std::move(vulkanBuffer));
    memories.push_back(std::move(vulkanMemory));
  }
  auto buffer = new VulkanBuffer(std::move(vulkanBuffers), std::move(memories));
  return buffer;
}
void tz::render::vulkan::VulkanRenderer::updateBuffer(tz::Buffer *buffer,
                                                      void *data,
                                                      size_t sizeInBytes)
{
  auto currentFrameBuffer = dynamic_cast<VulkanBuffer*>(buffer)->getMultiBufferByIndex(currentFrameIndex);
  auto& currentFrameBufferMem = dynamic_cast<VulkanBuffer*>(buffer)->getMultiMemoryByIndex(currentFrameIndex);
  auto targetMemory = currentFrameBufferMem.mapMemory(0, sizeInBytes);
  memcpy(targetMemory, data, sizeInBytes);
  currentFrameBufferMem.unmapMemory();
  // TODo we should maybe better permanently map certain buffers (via flag) but for now
  // we just stick with simple map/unmap.
}



void tz::render::vulkan::VulkanRenderer::createDescriptorPool()
{
  vk::DescriptorPoolSize poolSizes[3];
  poolSizes[0].setType(vk::DescriptorType::eUniformBuffer)
          .setDescriptorCount(100);
  poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler)
          .setDescriptorCount(500);
  poolSizes[2].setType(vk::DescriptorType::eStorageImage)
          .setDescriptorCount(50);

  vk::DescriptorPoolCreateInfo poolInfo;
  poolInfo.poolSizeCount = 3;
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
  poolInfo.maxSets = 200;

  descriptorPool = vk::raii::DescriptorPool(device, poolInfo);
}



/**
 * TODO: the singular buffer here is wrong - it must actually be an array of buffers and potentially imageViews as well. 
 * We need actually an association of a binding location with a "resource", which can be a buffer or an image view. So we need to pass in more information here, for example the actual descriptor set layout and the resources to bind. For now we just assume a single buffer for simplicity.
 * And then per such binding, we need to create a WriteDescriptorSet for each frame-in-flight, which points to the correct buffer for that frame. So we need to pass in the "multiFrameBuffer" here, which internally holds the actual buffers for each frame-in-flight 
 * and then we can create the correct WriteDescriptorSet for each frame.
 * The number of writeDescriptorSet definitions is frames-in-flight X number of bindings in the descriptor set layout. 
 * So we also need to pass in the descriptor set layout here, to know how many bindings we have and what types, 
 * to create the correct WriteDescriptorSet definitions.
 */
tz::DescriptorSet *tz::render::vulkan::VulkanRenderer::createMultiframeDescriptorSet(tz::DescriptorSetLayout* descriptorSetLayout)
{

  auto vdb = reinterpret_cast<VulkanDescriptorSetLayout*>(descriptorSetLayout);
  std::vector<vk::DescriptorSetLayout> layouts;
  std::vector<vk::Buffer> buffers;
  for (uint32_t i = 0; i< maxFramesInFlight; i++)
  {
    auto& dsl = vdb->dsLayout;
    layouts.push_back(dsl);
  }

  vk::DescriptorSetAllocateInfo allocateInfo;
  allocateInfo.setDescriptorPool(descriptorPool)
    .setDescriptorSetCount(maxFramesInFlight)
  .setSetLayouts(layouts);

  std::vector<vk::raii::DescriptorSet> descriptorSets;
  descriptorSets = device.allocateDescriptorSets(allocateInfo);

  for (uint32_t i = 0; i< maxFramesInFlight; i++)
  {

    // For each frame, we must prepare a WriteDescriptorSet that
    // points to the correct buffer for that frame, which is stored in the multiFrameBuffer.
    for (auto& binding : descriptorSetLayout->descriptorBindings) 
    {
      if (binding->type == tz::ResourceType::Sampler)
      {
        auto vulkanTexture = reinterpret_cast<VulkanTexture*>(binding->texture);
        auto& raiiSampler = vulkanTexture->getSampler();
        vk::DescriptorImageInfo descImageInfo;
        descImageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setImageView(vulkanTexture->getImageView())
        .setSampler(raiiSampler);


        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.setDstSet(descriptorSets[i])
          .setDstBinding(binding->bindingIndex)
          .setDstArrayElement(0)
          .setDescriptorCount(1)
          .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
          .setImageInfo(descImageInfo);
        device.updateDescriptorSets(writeDescriptorSet, {});
      }

      else if (binding->type == tz::ResourceType::Ubo || binding->type == tz::ResourceType::Ssbo)
      {
        vk::DescriptorBufferInfo descBufferInfo;
        auto vbuf = dynamic_cast<VulkanBuffer*>(binding->buffer);
        auto frameIndexBuffer = vbuf->getMultiBufferByIndex(i);
        descBufferInfo.setBuffer(frameIndexBuffer)
        .setOffset(0)
        .setRange(VK_WHOLE_SIZE);

        vk::WriteDescriptorSet writeDescriptorSet;
        writeDescriptorSet.setDstSet(descriptorSets[i])
          .setDstBinding(binding->bindingIndex)
          .setDstArrayElement(0)
          .setDescriptorCount(1)
            .setDescriptorType(vk::DescriptorType::eUniformBuffer)
          .setBufferInfo(descBufferInfo);
        device.updateDescriptorSets(writeDescriptorSet, {});
      }
    }
   
  }

  auto vulkanDescriptorWrapper = new tz::render::vulkan::VulkanDescriptorSet(std::move(descriptorSets));
  vulkanDescriptorWrapper->layout = descriptorSetLayout;
  return vulkanDescriptorWrapper;

}
tz::DescriptorSetLayout *tz::render::vulkan::VulkanRenderer::createDescriptorSetLayout(
  const std::vector<DescriptorBinding *>& bindings)
{
  std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
  for (auto& lb : bindings)
  {
    auto vulkanBinding = reinterpret_cast<VulkanDescriptorBinding*>(lb);
    auto descLayout = vulkanBinding->getDescLayout();
    layoutBindings.push_back(descLayout);
  }
  vk::DescriptorSetLayoutCreateInfo layoutInfo{};
  layoutInfo.setBindings(layoutBindings);
  auto descriptorLayout = vk::raii::DescriptorSetLayout(device, layoutInfo);

  auto dsLayout = new tz::render::vulkan::VulkanDescriptorSetLayout(std::move(descriptorLayout));
  dsLayout->descriptorBindings = bindings;
  return dsLayout;
}

vk::raii::CommandBuffer tz::render::vulkan::VulkanRenderer::beginOneTimeCommandbuffer()
{
  vk::CommandBufferAllocateInfo allocInfo;
  allocInfo.commandPool = commandPool;
  allocInfo.level = vk::CommandBufferLevel::ePrimary;
  allocInfo.commandBufferCount = 1;
  auto cb = std::move(device.allocateCommandBuffers(allocInfo).front());

  vk::CommandBufferBeginInfo beginInfo;
  beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  cb.begin(beginInfo);
  return cb;
}

void tz::render::vulkan::VulkanRenderer::endOneTimeCommandBuffer(vk::raii::CommandBuffer& cb)
{
  cb.end();
  vk::SubmitInfo submitInfo;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &*cb;
  graphicsQueue.submit(submitInfo, nullptr);
  graphicsQueue.waitIdle();


};

void tz::render::vulkan::VulkanRenderer::copyBuffer(vk::raii::Buffer& srcBuffer,
                                                    vk::raii::Buffer& targetBuffer, vk::DeviceSize size)
{
  vk::raii::CommandBuffer cb = beginOneTimeCommandbuffer();
  cb.copyBuffer(srcBuffer, targetBuffer, vk::BufferCopy(0, 0, size));
  endOneTimeCommandBuffer(cb);
}

tz::Texture *tz::render::vulkan::VulkanRenderer::createTexture(tz::Image *image)
{
  auto vImage = reinterpret_cast<tz::render::vulkan::VulkanImage*>(image);
  // First we create an image view:
  vk::ImageViewCreateInfo viewInfo;
  viewInfo.image = vImage->getImage();
  viewInfo.format = vk::Format::eR8G8B8A8Srgb;

  transitionImageLayout(vImage->getRaiiImage(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
  copyBufferToImage(vImage->getStagingBuffer(), vImage->getRaiiImage(), vImage->width, vImage->height);
  transitionImageLayout(vImage->getRaiiImage(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

  auto imageView = createVulkanImageView(vImage->getRaiiImage());

  vk::raii::Sampler sampler = reinterpret_cast<tz::render::vulkan::VulkanSampler*>(createSampler())->pullOutSampler();
  
  tz::render::vulkan::VulkanTexture* vulkanTexture = new tz::render::vulkan::VulkanTexture(std::move(imageView), std::move(sampler));
  return vulkanTexture;
}

tz::render::vulkan::VulkanBuffer* tz::render::vulkan::VulkanRenderer::createStagingBuffer(size_t sizeInBytes)
{
  vk::BufferCreateInfo createInfo;
  createInfo.setSize(sizeInBytes);
  createInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
  createInfo.setSharingMode(vk::SharingMode::eExclusive);
  auto b = vk::raii::Buffer(device, createInfo);

  // Allocating memory for the buffer and binding it to the buffer:
  vk::MemoryRequirements memRequirements = b.getMemoryRequirements();
  vk::MemoryAllocateInfo memoryAllocateInfo;
  memoryAllocateInfo.allocationSize = memRequirements.size;
  memoryAllocateInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                      vk::MemoryPropertyFlagBits::eHostVisible |
                                                vk::MemoryPropertyFlagBits::eHostCoherent);
  vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(device,memoryAllocateInfo);
  b.bindMemory(*bufferMemory, 0);

  return new VulkanBuffer(std::move(b), std::move(bufferMemory));
}

tz::Image *tz::render::vulkan::VulkanRenderer::createImage(tz::BitmapData bitmapData)
{
  vk::DeviceSize  imageSize = bitmapData.width * bitmapData.height * 4;
  auto stagingBuffer = createStagingBuffer(imageSize);
  void* targetData = stagingBuffer->getMemory().mapMemory(0, imageSize);
  memcpy(targetData, bitmapData.pixels, imageSize);
  stagingBuffer->getMemory().unmapMemory();
  // TODO we may actually cleanup the pixel array now. otoh we should not do this here!

  vk::raii::Image image = nullptr;
  vk::raii::DeviceMemory imageMemory = nullptr;

  vk::ImageCreateInfo imageInfo;
  imageInfo.imageType = vk::ImageType::e2D;
  imageInfo.format = vk::Format::eR8G8B8A8Srgb;
  imageInfo.extent = vk::Extent3D {bitmapData.width, bitmapData.height, 1};
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = vk::SampleCountFlagBits::e1;
  imageInfo.tiling = vk::ImageTiling::eOptimal;
  imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
  imageInfo.sharingMode = vk::SharingMode::eExclusive;
  image = vk::raii::Image(device, imageInfo);

  auto memReqs = image.getMemoryRequirements();
  vk::MemoryAllocateInfo allocInfo;
  allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = findMemoryType(memReqs.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
  imageMemory = vk::raii::DeviceMemory(device, allocInfo);
  image.bindMemory(imageMemory, 0);
  auto vulkanImage = new tz::render::vulkan::VulkanImage(std::move(image), std::move(imageMemory), std::move(stagingBuffer->pullOutBuffer()));
  vulkanImage->width = bitmapData.width;
  vulkanImage->height = bitmapData.height;
  return vulkanImage;
}

tz::ImageView *tz::render::vulkan::VulkanRenderer::createImageView(tz::Image *image)
{
  auto raiiVulkanImageView = createVulkanImageView(reinterpret_cast<VulkanImage*>(image)->getRaiiImage());
  auto vulkanImageView = new tz::render::vulkan::VulkanImageView(std::move(raiiVulkanImageView));
  return vulkanImageView;
  
}

tz::Sampler *tz::render::vulkan::VulkanRenderer::createSampler()
{
  vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
		vk::SamplerCreateInfo        samplerInfo{
		           .magFilter        = vk::Filter::eLinear,
		           .minFilter        = vk::Filter::eLinear,
		           .mipmapMode       = vk::SamplerMipmapMode::eLinear,
		           .addressModeU     = vk::SamplerAddressMode::eRepeat,
		           .addressModeV     = vk::SamplerAddressMode::eRepeat,
		           .addressModeW     = vk::SamplerAddressMode::eRepeat,
		           .mipLodBias       = 0.0f,
		           .anisotropyEnable = vk::True,
		           .maxAnisotropy    = properties.limits.maxSamplerAnisotropy,
		           .compareEnable    = vk::False,
		           .compareOp        = vk::CompareOp::eAlways};
		auto sampler = vk::raii::Sampler(device, samplerInfo);

    auto samplerWrapper = new tz::render::vulkan::VulkanSampler(std::move(sampler));
    return samplerWrapper;
}
