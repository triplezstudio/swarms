#pragma once
#include "common.hh"
#include "defines.h"
//#include "render.hh"
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

class VulkanDescriptorSet : public tz::DescriptorSet
{
  public:
      VulkanDescriptorSet(std::vector<vk::raii::DescriptorSet>&& descriptorSets) : descSets(std::move(descriptorSets)) {}

      std::vector<vk::raii::DescriptorSet> descSets;

};

class VulkanDescriptorSetLayout : public tz::DescriptorSetLayout
{
  public:
      VulkanDescriptorSetLayout(vk::raii::DescriptorSetLayout&& dsLayout)
      : dsLayout(std::move(dsLayout)) {}

      vk::raii::DescriptorSetLayout dsLayout;
};

class VulkanBuffer;
class VulkanImageView;
class VulkanDescriptorBinding : public tz::DescriptorBinding
{
  public:
      VulkanDescriptorBinding(vk::DescriptorSetLayoutBinding&& descriptorSetLayout)
        : descSetLayoutBinding(std::move(descriptorSetLayout)) {}

      vk::DescriptorSetLayoutBinding getDescLayout()
      {
        return descSetLayoutBinding;
      }

  vk::DescriptorSetLayoutBinding descSetLayoutBinding;
};

class VulkanTexture : public tz::Texture
{
  public:
      VulkanTexture(vk::raii::ImageView&& imageView, vk::raii::Sampler&& sampler)
      : raiiImageView(std::move(imageView)), sampler(std::move(sampler)) {}

      vk::raii::ImageView& getImageView()
      {
        return raiiImageView;
      }

      vk::raii::Sampler& getSampler()
      {
        return sampler;
      }

  private:
      vk::raii::ImageView raiiImageView;
      vk::raii::Sampler sampler;
};

class VulkanSampler : public tz::Sampler
{
  public:
      VulkanSampler(vk::raii::Sampler&& sampler) : sampler(std::move(sampler)) {}

      vk::raii::Sampler& getSampler()
      {
        return sampler;
      }

      vk::raii::Sampler&& pullOutSampler()
      {
        return std::move(sampler);
      }

  private:
      vk::raii::Sampler sampler;
};

class VulkanImageView : public tz::ImageView
{

  public:
      VulkanImageView(vk::raii::ImageView&& imageView) : imageView(std::move(imageView)) {}

      vk::raii::ImageView& getImageView()
      {
        return imageView;
      }

      vk::raii::ImageView&& pullOutImageView()
      {
        return std::move(imageView);
      }

  private:
      vk::raii::ImageView imageView;
};

class VulkanImage : public tz::Image
{
  public:
      VulkanImage(vk::raii::Image&& image, vk::raii::DeviceMemory&& imageMemory, vk::raii::Buffer&& stagingBuffer)
      : image(std::move(image)), imageMemory(std::move(imageMemory)), stagingBuffer(std::move(stagingBuffer)) {}

      vk::raii::Image& getRaiiImage()
      {
        return image;
      }

      vk::Image getImage()
      {
        return *image;
      }

      vk::raii::Buffer& getStagingBuffer()
      {
        return stagingBuffer;
      }

      vk::DeviceMemory getMemory()
      {
        return *imageMemory;
      }

  private:
      vk::raii::Image image;
      vk::raii::DeviceMemory imageMemory;
      vk::raii::Buffer stagingBuffer;
};

class VulkanBuffer : public tz::Buffer
{

  public:
  VulkanBuffer(vk::raii::Buffer&& vkBuffer, vk::raii::DeviceMemory&& devMemory)
  {
    buffer = std::move(vkBuffer);
    devMemory = std::move(memory);
  }

  VulkanBuffer(std::vector<vk::raii::Buffer>&& multiBuffers, std::vector<vk::raii::DeviceMemory>&& multiMemories)
  {
    this->multiBuffers = std::move(multiBuffers);
    this->multiMemories = std::move(multiMemories);
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

  vk::raii::Buffer&& pullOutBuffer()
  {
    return std::move(buffer);
  }

  vk::raii::DeviceMemory&& pullOutMemory()
  {
    return std::move(memory);
  }

  vk::Buffer getBuffer()
  {
    return *buffer;
  }

  vk::Buffer getMultiBufferByIndex(int index)
  {
    return *multiBuffers[index];
  }

  vk::raii::DeviceMemory& getMultiMemoryByIndex(int index)
  {
    return multiMemories[index];
  }

  vk::DeviceMemory getMemoryHandle()
  {
    return *memory;
  }

  vk::raii::DeviceMemory& getMemory()
  {
    return memory;
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

      // These multibuffers are useful to have
      // one logical buffer which cover n buffers, e.g. one for each frame in flight.
      // So the client only must deal with one buffer at any time, but inside
      // the engine, the current frame-index buffer can be used.
      std::vector<vk::raii::Buffer> multiBuffers;
      std::vector<vk::raii::DeviceMemory> multiMemories;
};

struct VulkanInitData
{
  client_common::NativeHandles nativeHandles;
  std::vector<const char*> extensions;
  std::optional<std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)>> surfaceCreationFunc;
  std::function<void(int* width, int* height)> displaySizeFunc;
};

struct VulkanPipelineLayout : public PipelineLayout
{
  VulkanPipelineLayout(vk::raii::PipelineLayout&& pl) : pipelineLayout(std::move(pl)) {}

  vk::raii::PipelineLayout pipelineLayout;
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
  Buffer * createMultiframeBuffer(void *initialData, size_t sizeInBytes, tz::BufferUsage bufferUsage) override;
  Buffer * createMultiframeUniformBuffer(uint32_t numberOfPlannedObjects, size_t objectSize) override;
  DescriptorBinding * createDescriptorBinding(uint8_t binding,
                                             tz::ResourceType resourceType,
                                             tz::ShaderType shaderType,
                                             uint32_t count,
                                             Buffer* buffer = nullptr,
                                             Texture* texture = nullptr) override;
  DescriptorSetLayout * createDescriptorSetLayout(const std::vector<DescriptorBinding *> &bindings, bool bindless = false) override;
  void updateTextureDescriptorSet(tz::DescriptorSet *pSet, int binding, int index, tz::Texture *pTexture) override;
  ShaderModule* createShaderModule(tz::ShaderType type, const std::string &source) override;
  ShaderPipeline * createShaderPipeline(const std::vector<ShaderModule *> &modules) override;
  CommandBuffer * createCommandBuffer() override;
  DescriptorSet * createMultiframeDescriptorSet(DescriptorSetLayout* descriptorSetLayout) override;

  Texture * createTexture(Image* image) override;
  Image * createImage(tz::BitmapData bitmapData) override;
  ImageView * createImageView(Image* image) override;
  Sampler * createSampler() override;
  void beginCommandBuffer(tz::CommandBuffer *cb) override;
  void endCommandBuffer(tz::CommandBuffer *cb) override;
  void recordCommand(tz::CommandBuffer* cb, tz::Command *cmd) override;
  PipelineLayout * createPipelineLayout(std::vector<tz::DescriptorSetLayout *> descriptorSetLayouts) override;
  PipelineStateObject * createPipelineStateObject(tz::RenderState &renderState, tz::ShaderPipeline *shaderPipeline, tz::VertexLayout &vertexLayout,  PipelineLayout* providedPipelineLayout) override;

  vk::raii::PipelineLayout createPipelineLayout(std::vector<vk::DescriptorSetLayout> descriptorSetLayouts);

  private:
  void initSurface();
  void createInstance();
  void enableValidationLayers();
  void pickPhysicalDevice();
  void createLogicalDevice();
  void createSwapChain();
  void createImageViews();
  vk::raii::ImageView createVulkanImageView(vk::raii::Image& image);
  void createGraphicsPipeline();
  void createCommandPool();
  void createDefaultCommandBuffer();
  void recordDefaultCommandBuffer();
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
  void updateBuffer(tz::Buffer *buffer, void *data, size_t sizeInBytes, uint32_t offset) override;

  private:
  const uint32_t maxFramesInFlight = 2;
  tz::Window* window = nullptr;
  vk::raii::Context context;
  vk::raii::Instance instance = nullptr;
  vk::raii::SurfaceKHR surface = nullptr;
  vk::raii::PhysicalDevice physicalDevice = nullptr;
  vk::raii::Device device = nullptr;
  vk::raii::Queue graphicsQueue = nullptr;
  vk::raii::DescriptorPool descriptorPool = nullptr;
  vk::Extent2D swapExtent;
  vk::SurfaceFormatKHR surfaceFormat;
  vk::raii::SwapchainKHR swapChain = nullptr;
  vk::raii::PipelineLayout pipelineLayout = nullptr;
  vk::raii::Pipeline graphicsPipeline = nullptr;
  vk::raii::CommandPool commandPool = nullptr;
  vk::raii::CommandBuffer commandBuffer = nullptr;

  size_t minUniformBufferOffsetAlignment = 0;

  uint32_t currentFrameIndex = 0;
  uint32_t imageIndex;

  // Synchronization primitives:
  std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
  std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
  std::vector<vk::raii::Fence> drawFences;

  uint32_t graphicsQueueIndex = 0;
  std::vector<vk::Image> swapChainImages;
  std::vector<vk::raii::ImageView> swapChainImageViews;
  std::vector<const char*> extensions;
  client_common::NativeHandles nativeHandles;
  VulkanInitData vulkanInitData;
  std::vector<char const*> requiredLayers;
  vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
  vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features,
                     vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
                     vk::PhysicalDeviceShaderDrawParametersFeatures,
                     vk::PhysicalDeviceVulkan12Features> deviceFeatures;
  std::map<CommandBuffer*, vk::raii::CommandBuffers*> customCommandBuffers;

  std::vector<vk::VertexInputAttributeDescription> toVulkanAttributeDescriptions(
    const std::vector<tz::VertexAttribute> &vertexAttributes);
  vk::Format toVulkanAttributeFormat(VertexAttribute va);
  uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags);
  void createDescriptorPool();
  void transitionImageLayout(CommandBuffer *cb,
                             vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout,
                             vk::AccessFlags2 srcAccessMask,
                             vk::AccessFlags2 dstAccessMask,
                             vk::PipelineStageFlags2 srcStageMask,
                             vk::PipelineStageFlags2 dstStageMask);
  VulkanBuffer *createStagingBuffer(size_t sizeInBytes);
  vk::raii::CommandBuffer beginOneTimeCommandbuffer();
  void endOneTimeCommandBuffer(vk::raii::CommandBuffer &cb);
  void copyBuffer(vk::raii::Buffer &srcBuffer, vk::raii::Buffer &targetBuffer, vk::DeviceSize size);
  void transitionImageLayout(const vk::raii::Image &image,
                             vk::ImageLayout oldLayout,
                             vk::ImageLayout newLayout);
  void copyBufferToImage(const vk::raii::Buffer &buffer,
                         vk::raii::Image &image,
                         uint32_t width,
                         uint32_t height);
  uint32_t getAlignedStride(size_t size, uint32_t minAlignment);
  vk::FrontFace toVulkanFrontFace(FrontFace frontFace);
  vk::CullModeFlags toVulkanCullMode(CullMode cullMode);
};

vk::DescriptorType toVulkanDescriptorType(ResourceType resourceType);
vk::ShaderStageFlagBits toShaderStageFlags(ShaderType shaderType);
} // namespace tz::render::vulkan