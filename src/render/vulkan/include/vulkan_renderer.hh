#pragma once
#include "common.hh"
#include "defines.h"
#include <Eigen/Dense>
#include <functional>
#include <optional>
#include <map>
#include <vulkan/vulkan_raii.hpp>
#include <window_system.hh>

namespace tz::render::vulkan {



enum class BufferUsage
{
  Vertex,
  Index,
  Uniform,
  Storage,
  TansferDest,
  TransferSource,
  Indirect,

};


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



struct ViewPort
{
  int x;
  int y;
  int width;
  int height;
};

struct Scissor
{
  int x;
  int y;
  int width;
  int height;
};


class PipelineStateObject;
class Command
{
  public:
  virtual ~Command() = default;
};

class CmdBindPipeline : public Command
{
  public:
  CmdBindPipeline(PipelineStateObject* pso) : pso(pso) {}

  public:
  PipelineStateObject* pso = nullptr;
};

class DescriptorSet;
class PipelineLayout;
class CmdBindDescriptors : public Command
{
  public:
  CmdBindDescriptors(std::vector<DescriptorSet*> descriptorSets, PipelineLayout* pl, std::vector<uint32_t> offsets, uint32_t setIndex)
    : descriptorSets(descriptorSets), pipelineLayout(pl), offsets(offsets), setIndex(setIndex) {}

  std::vector<DescriptorSet*> descriptorSets;
  PipelineLayout* pipelineLayout;
  std::vector<uint32_t> offsets;
  uint32_t setIndex;
};


class CmdSetViewPorts : public Command
{
  public:
  CmdSetViewPorts(std::vector<ViewPort> viewPorts) : viewPorts(viewPorts) {}

  std::vector<ViewPort> viewPorts;
};

class CmdSetScissors : public Command
{
  public:
  CmdSetScissors(std::vector<Scissor> scissors) : scissors(scissors) {}

  std::vector<Scissor> scissors;
};

class Buffer;
class CmdBindIndexBuffer : public Command
{
  public:
  CmdBindIndexBuffer(Buffer* buffer, uint32_t offset) : indexBuffer(buffer), offset(offset) {}

  Buffer* indexBuffer;
  uint32_t offset;
};

class CmdBindVertexBuffers : public Command
{
  public:
  // TODO: we "must" copy the vbs here, maybe we can avoid this.
  CmdBindVertexBuffers(std::vector<Buffer*> vbs) : vertexBuffers(vbs) {}

  std::vector<Buffer*> vertexBuffers;
};

class CmdDrawIndexed : public Command
{
  public:
  CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                 uint32_t vertexOffset, uint32_t firstInstance) :
    indexCount(indexCount), instanceCount(instanceCount), firstIndex(firstIndex),
    vertexOffset(vertexOffset), firstInstance(firstInstance) {}

  uint32_t indexCount;
  uint32_t instanceCount;
  uint32_t firstIndex;
  uint32_t vertexOffset;
  uint32_t firstInstance;

};

class CmdDraw : public Command
{
  public:
  CmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) :
    vertexCount(vertexCount), instanceCount(instanceCount), firstVertex(firstVertex), firstInstance(firstInstance)
  {}

  uint32_t vertexCount;
  uint32_t instanceCount;
  uint32_t firstVertex;
  uint32_t firstInstance;
};



class CommandBuffer
{

  public:
  explicit CommandBuffer(vk::raii::CommandBuffers&& cbs) {
    this->cbs = std::move(cbs);
  }

  void * getHandle()
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

enum class ShaderType {
  Vertex,
  Fragment,
  Tessellation
};

class ShaderModule
{

  public:
  ShaderModule(vk::PipelineShaderStageCreateInfo createInfo, vk::raii::ShaderModule&& shaderModule):
        createInfo(createInfo), shaderModule(std::move(shaderModule)) {}


  vk::PipelineShaderStageCreateInfo createInfo;

  private:

      vk::raii::ShaderModule shaderModule;

};

class ShaderPipeline
{
  public:

  void link(const std::vector<ShaderModule *> &modules)
  {
    stages = modules;
  }
  void * getHandle()
  {
    return &stages;
  }

  private:
  std::vector<ShaderModule*> stages;
};





enum class DescriptorResourceType
{
  Ubo,
  Ssbo,
  Sampler
};


class Buffer;
class Texture;
class DescriptorBinding
{
  public:
      DescriptorBinding(vk::DescriptorSetLayoutBinding&& descriptorSetLayout)
        : descSetLayoutBinding(std::move(descriptorSetLayout)) {}

      vk::DescriptorSetLayoutBinding getDescLayout()
      {
        return descSetLayoutBinding;
      }

      struct Details
      {
        uint8_t setIndex     = 0;
        uint8_t bindingIndex = 0;
        DescriptorResourceType type;
        ShaderType shaderType;
        uint32_t count = 1; // This is useful for arrays/instancing
        Buffer *buffer
          = nullptr; // This is optional, but useful to store here for later descriptor set creation
        Texture *texture = nullptr; // Optional, needed when image-textures are used:
      };

      DescriptorResourceType type;
      Buffer* buffer = nullptr;
      Texture* texture= nullptr;
      ShaderType shaderType;
      uint32_t count = 0;
      uint32_t bindingIndex = 0;

  private:
  vk::DescriptorSetLayoutBinding descSetLayoutBinding;
};

class DescriptorSetLayout
{
  public:
  DescriptorSetLayout(vk::raii::DescriptorSetLayout&& dsLayout)
    : dsLayout(std::move(dsLayout)) {}

  vk::raii::DescriptorSetLayout dsLayout;

  std::vector<DescriptorBinding*> descriptorBindings;
};

class DescriptorSet
{
  public:
  DescriptorSet(std::vector<vk::raii::DescriptorSet>&& descriptorSets) : descSets(std::move(descriptorSets)) {}

  std::vector<vk::raii::DescriptorSet> descSets;

  DescriptorSetLayout* layout;

};

struct BitmapData
{
  uint8_t* pixels;
  uint32_t width;
  uint32_t height;
};

BitmapData TZ_API loadBitmapDataFromPath(const std::string & imagePath);

class Texture
{
  public:
      Texture(vk::raii::ImageView&& imageView, vk::raii::Sampler&& sampler)
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

class Sampler
{
  public:
      Sampler(vk::raii::Sampler&& sampler) : sampler(std::move(sampler)) {}

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

class ImageView
{

  public:
      ImageView(vk::raii::ImageView&& imageView) : imageView(std::move(imageView)) {}

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

class Image
{
  public:
      Image(vk::raii::Image&& image, vk::raii::DeviceMemory&& imageMemory, vk::raii::Buffer&& stagingBuffer)
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

      struct Details
      {
        uint8_t* pixels;
        size_t size = 0;
        uint32_t width = 0;
        uint32_t height = 0;
      } details;

  private:
      vk::raii::Image image;
      vk::raii::DeviceMemory imageMemory;
      vk::raii::Buffer stagingBuffer;


};

class Buffer
{

  public:
  Buffer(vk::raii::Buffer&& vkBuffer, vk::raii::DeviceMemory&& devMemory)
  {
    buffer = std::move(vkBuffer);
    devMemory = std::move(memory);
  }

  Buffer(std::vector<vk::raii::Buffer>&& multiBuffers, std::vector<vk::raii::DeviceMemory>&& multiMemories)
  {
    this->multiBuffers = std::move(multiBuffers);
    this->multiMemories = std::move(multiMemories);
  }

  void updateData(void* data, size_t sizeInBytes)
  {
    
  }

  void appendData(void* data, size_t sizeInBytes)
  {
    
  }
  
  void* getHandle()
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


    size_t unitSize = 0;
    size_t overallSize = 0;


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

struct PipelineLayout
{
  PipelineLayout(vk::raii::PipelineLayout&& pl) : pipelineLayout(std::move(pl)) {}

  vk::raii::PipelineLayout pipelineLayout;
};

struct PipelineStateObject
{
  PipelineStateObject(vk::raii::Pipeline&& pipeline)
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

enum class CullMode
{
  None,
  Front,
  Back
};

enum class FillMode
{
  Solid,
  Line
};

enum class FrontFace
{
  Clockwise,
  CounterClockwise
};

enum class PrimitiveType {
  Triangles,
  Lines,
  Quads

};


enum class VertexInputRate
{
  PerVertex,
  PerInstance
};

enum class DataType
{
  Byte,
  Short,
  Int,
  UInt,
  Float,
  Double,

};


struct VertexBinding
{
  uint32_t bufferSlot;
  uint32_t stride;
  VertexInputRate vertexInputRate;

};

struct VertexAttribute
{
  uint32_t shaderLocation;
  uint32_t bufferSlot;
  DataType dataType;
  int32_t componentCount;
  uint32_t offset;


};

struct VertexLayout
{
  std::vector<VertexBinding> bindings;
  std::vector<VertexAttribute> attributes;

  std::string toHash()
  {
    std::string hash = "";
    for (auto& b : bindings)
    {
      hash += std::to_string(b.bufferSlot);
      hash += std::to_string(b.stride);
      hash += std::to_string(static_cast<int>(b.vertexInputRate));
    }
    for (auto& a : attributes)
    {
      hash += std::to_string(a.bufferSlot);
      hash += std::to_string(a.shaderLocation);
      hash += std::to_string(a.offset);
      hash += std::to_string(a.componentCount);
      hash += std::to_string(static_cast<int>(a.dataType));
    }

    return hash;
  }
};




struct RenderState
{
  vk::CullModeFlags cullMode = vk::CullModeFlagBits::eBack;
  vk::PolygonMode fillMode = vk::PolygonMode::eFill;
  vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;
  bool depthTesting = false;
  bool stencilTesting = false;
  bool blending = false;
  PrimitiveType primitiveType = PrimitiveType::Triangles;
};





struct VertexPos
{
  Eigen::Vector3f pos;
};

struct VertexPosColor
{
  Eigen::Vector3f pos;
  Eigen::Vector3f color;
};

struct VertexPosTexCoords
{
  Eigen::Vector3f pos;
  Eigen::Vector2f texCoords;
};


/**
 * Renderer .
 *
 */
class TZ_API Renderer
{
  public:
  void init(tz::Window *window);

  WindowDesc getRequiredWindowDesc() ;

  void beginFrame() ;
  void endFrame() ;
  void submitCommandBuffer(CommandBuffer*cb);

  Buffer* createBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage);
  Buffer * createMultiframeBuffer(void *initialData, size_t sizeInBytes, BufferUsage bufferUsage);
  Buffer * createMultiframeUniformBuffer(uint32_t numberOfPlannedObjects, size_t objectSize);
  DescriptorBinding * createDescriptorBinding(uint8_t binding,
                                             DescriptorResourceType resourceType,
                                             ShaderType shaderType,
                                             uint32_t count,
                                             Buffer* buffer = nullptr,
                                             Texture* texture = nullptr) ;
  DescriptorSetLayout * createDescriptorSetLayout(const std::vector<DescriptorBinding *> &bindings, bool bindless = false) ;
  void updateTextureDescriptorSet(DescriptorSet *pSet, int binding, int index, Texture *pTexture);
  ShaderModule* createShaderModule(ShaderType type, const std::string &source) ;
  ShaderPipeline * createShaderPipeline(const std::vector<ShaderModule *> &modules) ;
  CommandBuffer * createCommandBuffer();
  DescriptorSet * createMultiframeDescriptorSet(DescriptorSetLayout* descriptorSetLayout) ;

  Texture * createTexture(Image* image);
  Image * createImage(BitmapData bitmapData);
  ImageView * createImageView(Image* image) ;
  Sampler * createSampler() ;
  void beginCommandBuffer(CommandBuffer *cb);
  void endCommandBuffer(CommandBuffer *cb);
  void recordCommand(CommandBuffer* cb, Command *cmd);
  PipelineLayout * createPipelineLayout(std::vector<DescriptorSetLayout *> descriptorSetLayouts);
  PipelineStateObject * createPipelineStateObject(RenderState &renderState, ShaderPipeline *shaderPipeline, VertexLayout &vertexLayout,  PipelineLayout* providedPipelineLayout);

  vk::raii::PipelineLayout createPipelineLayout(std::vector<vk::DescriptorSetLayout> descriptorSetLayouts);

  void updateBuffer(Buffer *buffer, void *data, size_t sizeInBytes, uint32_t offset);

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
  std::vector<vk::VertexInputBindingDescription> toVulkanBindingDescriptions(const std::vector<VertexBinding>& vertexBindings);
  vk::VertexInputRate toVulkanInputRate(VertexInputRate ir);
  void createSyncObjects();
  vk::raii::CommandBuffer& getCommandBufferForCurrentFrame(CommandBuffer* cb);
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
    const std::vector<VertexAttribute> &vertexAttributes);
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
  Buffer *createStagingBuffer(size_t sizeInBytes);
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

vk::DescriptorType toVulkanDescriptorType(DescriptorResourceType resourceType);
vk::ShaderStageFlagBits toShaderStageFlags(ShaderType shaderType);
} // namespace tz::render::vulkan