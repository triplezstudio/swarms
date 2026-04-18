#pragma once
#include "common.hh"
#include "defines.h"
#include "render.hh"
#include <functional>
#include <optional>
#include <map>
#include <vulkan/vulkan_raii.hpp>

namespace tz::render::vulkan {

inline vk::BufferUsageFlags toVkBufferUsage(BufferUsage bufferUsage)
{
  vk::BufferUsageFlags flags;
  switch (bufferUsage)
  {
    case BufferUsage::Vertex: flags |= vk::BufferUsageFlagBits::eVertexBuffer; break;
    case BufferUsage::Index: flags |= vk::BufferUsageFlagBits::eIndexBuffer; break;
    case BufferUsage::Indirect: flags |= vk::BufferUsageFlagBits::eIndirectBuffer; break;
    case BufferUsage::Storage: flags |= vk::BufferUsageFlagBits::eStorageBuffer; break;
    case BufferUsage::TansferDest: flags |= vk::BufferUsageFlagBits::eTransferDst; break;
    case BufferUsage::TransferSource: flags |= vk::BufferUsageFlagBits::eTransferSrc; break;
    case BufferUsage::Uniform: flags |= vk::BufferUsageFlagBits::eUniformBuffer; break;
  }
  return flags;
}

class VulkanCommandBuffer : public tz::CommandBuffer
{

  public:
  explicit VulkanCommandBuffer(vk::raii::CommandBuffers&& cbs) {
    this->cbs = std::move(cbs);
  }



  void * getHandle() override
  {
    return &cbs;
  }

  vk::raii::CommandBuffer& getCommandBufferForImage(int imageIndex)
  {
    return cbs[imageIndex];
  }

  private:
      vk::raii::CommandBuffers cbs = nullptr;
};

class VulkanShaderPipeline : public tz::ShaderPipeline
{
  public:

      void link(const std::vector<ShaderModule *> &modules) override
      {
        stages = modules;
      }
      void * getHandle() override
      {
        return &stages;
      }

  private:
      std::vector<ShaderModule*> stages;
};

class VulkanShaderModule : public tz::ShaderModule
{

  public:
  VulkanShaderModule(vk::PipelineShaderStageCreateInfo createInfo, vk::raii::ShaderModule&& shaderModule):
        createInfo(createInfo), shaderModule(std::move(shaderModule)) {}
  void init(ShaderType type, const std::string& source) override {}
  void* getHandle() override
  {
    return &createInfo;
  }

  private:
      vk::PipelineShaderStageCreateInfo createInfo;
      vk::raii::ShaderModule shaderModule;

};



class VulkanBuffer : public tz::Buffer
{

  public:
  VulkanBuffer(vk::raii::Buffer&& vkBuffer, vk::raii::DeviceMemory&& devMemory)
  {
    buffer = std::move(vkBuffer);
    devMemory = std::move(memory);
  }

  void updateData(void* data, size_t sizeInBytes) override
  {
    
  }

  void appendData(void* data, size_t sizeInBytes) override
  {
    
  }
  
  void* getHandle() override
  {
    return (void *) &(*buffer);
  }

  vk::Buffer getBuffer()
  {
    return *buffer;
  }

  void bind() override
  {

  }
  void unbind() override
  {

  }
  void map() override
  {

  }
  void unmap() override
  {

  }
  private:
      vk::raii::Buffer buffer = nullptr;
      vk::raii::DeviceMemory memory = nullptr;
};

struct VulkanInitData
{
  client_common::NativeHandles nativeHandles;
  std::vector<const char*> extensions;
  std::optional<std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)>> surfaceCreationFunc;
  std::function<void(int* width, int* height)> displaySizeFunc;
};

struct VulkanPSO : public PipelineStateObject
{

  VulkanPSO(vk::raii::Pipeline&& pipeline)
  {
    this->pipeline = std::move(pipeline);
  }

  vk::raii::Pipeline& getHandle()
  {
    return pipeline;
  }

  private:
      vk::raii::Pipeline pipeline = nullptr;
};

/**
 * VulkanRenderer is a custom renderer using the Vulkan API.
 *
 */
class TZ_API VulkanRenderer : public Renderer
{
  public:
  void init(tz::Window *window) override;

  WindowDesc getRequiredWindowDesc() override;
  void clearScreen() override;
  void beginDraw(PrimitiveType primitiveType) override;
  void endDraw() override;
  void emitPosition(Eigen::Vector3f position) override;
  void emitColor(Eigen::Vector4f color) override;
  void emitUV(Eigen::Vector2f uv) override;
  void emitNormal(Eigen::Vector3f normal) override;

  void beginFrame() override;
  void endFrame() override;
  void submitCommandBuffer(CommandBuffer*cb) override;

  Buffer* createBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage) override;
  ShaderModule* createShaderModule(tz::ShaderType type, const std::string &source) override;
  ShaderPipeline * createShaderPipeline(const std::vector<ShaderModule *> &modules) override;
  CommandBuffer * createCommandBuffer() override;
  void beginCommandBuffer(tz::CommandBuffer *cb) override;
  void endCommandBuffer(tz::CommandBuffer *cb) override;
  void recordCommand(tz::CommandBuffer* cb, tz::Command *cmd) override;
  PipelineStateObject * createPipelineStateObject(tz::RenderState &renderState,
                                                 tz::ShaderPipeline *shaderPipeline,
                                                 tz::VertexLayout &vertexLayout,
                                                 const std::vector<DescriptorBinding> &descriptorBindings) override;

  private:
  void initSurface();
  void createInstance();
  void enableValidationLayers();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  void createGraphicsPipeline();
  void createCommandPool();
  void createDefaultCommandBuffer();
  void recordCommandBuffer(uint32_t imageIndex);
  void transitionImageLayout( uint32_t imageIndex,
                             vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout,
                             vk::AccessFlags2 srcAccessMask,
                             vk::AccessFlags2 dstAccessMask,
                             vk::PipelineStageFlags2 srcStageMask,
                             vk::PipelineStageFlags2 dstStageMask);
  std::vector<vk::VertexInputBindingDescription> toVulkanBindingDescriptions(const std::vector<tz::VertexBinding>& vertexBindings);
  vk::VertexInputRate toVulkanInputRate(VertexInputRate ir);
  void createSyncObjects();
  vk::raii::CommandBuffer& getCommandBufferForCurrentFrame(tz::CommandBuffer* cb);
  vk::raii::ShaderModule createSlangShaderModule(const std::string& shaderBinaryPath);
  bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice);
  vk::SurfaceFormatKHR selectSurfaceColorFormat(std::vector<vk::SurfaceFormatKHR> const& availableFormats);
  vk::PresentModeKHR selectSwapPresentMode(std::vector<vk::PresentModeKHR> const& modes);
  vk::Extent2D selectSwapExtent(vk::SurfaceCapabilitiesKHR const & capabilities);
  uint32_t selectSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& caps);
  void setupDebugMessenger();
  static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT       severity,
                                                        vk::DebugUtilsMessageTypeFlagsEXT              type,
                                                        const vk::DebugUtilsMessengerCallbackDataEXT * pCallbackData,
                                                        void *                                         pUserData);

  private:
  const uint32_t maxFramesInFlight = 2;
  tz::Window* window = nullptr;
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::SurfaceKHR surface = nullptr;
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  vk::raii::Device device = nullptr;
  vk::raii::Queue graphicsQueue = nullptr;
  vk::Extent2D swapExtent;
  vk::SurfaceFormatKHR surfaceFormat;
  vk::raii::SwapchainKHR swapChain = nullptr;
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::Pipeline graphicsPipeline = nullptr;
  vk::raii::CommandPool commandPool = nullptr;
  vk::raii::CommandBuffer commandBuffer = nullptr;

  uint32_t imageIndex = 0;

  // Synchronization primitives:
  vk::raii::Semaphore presentCompleteSemaphore = nullptr;
  vk::raii::Semaphore renderFinishedSemaphore = nullptr;
  vk::raii::Fence drawFence = nullptr;

  uint32_t graphicsQueueIndex = 0;
  std::vector<vk::Image> swapChainImages;
  std::vector<vk::raii::ImageView> swapChainImageViews;
  std::vector<const char*> extensions;
  client_common::NativeHandles nativeHandles;
  VulkanInitData vulkanInitData;
  std::vector<char const*> requiredLayers;
  vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
  vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> deviceFeatures;
  std::map<CommandBuffer*, vk::raii::CommandBuffers*> customCommandBuffers;

  std::vector<vk::VertexInputAttributeDescription> toVulkanAttributeDescriptions(
    const std::vector<tz::VertexAttribute> &vertexAttributes);
  vk::Format toVulkanAttributeFormat(VertexAttribute va);
  uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags);
  void transitionImageLayout(CommandBuffer *cb,
                             vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout,
                             vk::AccessFlags2 srcAccessMask,
                             vk::AccessFlags2 dstAccessMask,
                             vk::PipelineStageFlags2 srcStageMask,
                             vk::PipelineStageFlags2 dstStageMask);
};

}