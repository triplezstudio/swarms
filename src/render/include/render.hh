#pragma once

#include <defines.h>
#include <Eigen/Dense>
#include <window_system.hh>
#include <array>


/**
 * The render interface.
 * It abstracts over any underlying render api, e.g. Vulkan, OpenGL
 * or a 3rd party render library.
 * In any case we want to be able to
 * a) switch render backends in the future if specific features are needed
 * b) be able to provide platform optimal render backends so we need to be able to support
 *    multiple renderers behind this interface anyway.
 */
namespace tz {



enum class CameraType
{
  Ortho,
  Perspective
};

class Camera
{
  public:
  Camera(Eigen::Vector3f pos, Eigen::Vector3f lookAt, CameraType type) : pos(pos), lookAt(lookAt), type(type)
  {

  }

  Eigen::Matrix4f getViewMatrix()
  {
    Eigen::Vector3f up = {0, 1, 0};
    Eigen::Vector3f f = (lookAt - pos).normalized();
    Eigen::Vector3f s = f.cross(up).normalized();
    Eigen::Vector3f u = s.cross(f);

    Eigen::Matrix4f mat = Eigen::Matrix4f::Identity();

    // Set Columns (Eigen is Column-Major)
    mat.col(0).head<3>() = s;
    mat.col(1).head<3>() = u;
    mat.col(2).head<3>() = -f;

    // Translation part
    mat(0,3) = -s.dot(pos);
    mat(1,3) = -u.dot(pos);
    mat(2,3) =  f.dot(pos);

    return mat;
  }

  Eigen::Matrix4f getProjectionMatrix(float width, float height)
  {
    if (type == CameraType::Perspective)
    {
      float fovY = 0.5236; // around 30 degress vertical fov
      float aspect = width / height;
      float tanHalfFovy = std::tan(fovY * 0.5f);
      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      // TODO customizable:

      float zFar = 1000;
      float zNear = 0.1;

      m(0,0) = 1.0f / (aspect * tanHalfFovy);
      m(1,1) = 1.0f / (tanHalfFovy); // Negated for Vulkan Y-down
      m(2,2) = zFar / (zNear - zFar);
      m(2,3) = (zNear * zFar) / (zNear - zFar);
      m(3,2) = -1.0f; // This must be at (3,2) for Eigen's Col-Major layout

      return m;
    }
    else if (type == CameraType::Ortho)
    {
      float left = 0;
      float right = width;
      float top = height;
      float bottom = 0;

      // TODO customizable:
      float zFar = 100;
      float zNear = 0.1;

      Eigen::Matrix4f m = Eigen::Matrix4f::Zero();

      m(0,0) = 2.0f / (right - left);
      m(1,1) = 2.0f / (top - bottom);
      m(2,2) = 1.0f / (zNear - zFar);   // Vulkan: [0,1] depth
      m(3,3) = 1.0f;

      m(0,3) = -(right + left) / (right - left);
      m(1,3) = -(top + bottom) / (top - bottom);
      m(2,3) = zNear / (zNear - zFar);

      return m;
    }
  }


  public:
  Eigen::Vector3f pos;
  Eigen::Vector3f lookAt;
  CameraType type;

};


enum class PrimitiveGeometryType
{
  Line,
  Quad,
  Cube,
  Sphere,
  Mesh
};

enum class PrimitiveMaterialType
{
  SingleColor,
  DiffuseTexture,
  PBR,

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

struct CameraUniformBufferObject
{
  Eigen::Matrix4f view;
  Eigen::Matrix4f proj;
};

struct alignas(16) TransformUniformBufferObject
{
  Eigen::Matrix4f model;

};

struct alignas(16) PerObjectUniformBufferObject
{
  Eigen::Matrix4f model;
  uint32_t textureId;
  uint32_t padding[3];

};

struct Transform
{
  Eigen::Vector3f position = {0, 0, 0};
  Eigen::Vector3f scale = {1, 1, 1};
  Eigen::Quaternionf orientation;
};

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

struct Buffer
{
  virtual void updateData(void* data, size_t sizeInBytes)= 0;
  virtual void appendData(void* data, size_t sizeInBytes) = 0;
  virtual void* getHandle() = 0;
  virtual void bind() = 0;
  virtual void unbind() = 0;
  virtual void map() = 0;
  virtual void unmap() = 0;

  size_t unitSize = 0;
  size_t overallSize = 0;

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

enum class ResourceType
{
  Ubo,
  Ssbo,
  Sampler
};

enum class ShaderType {
  Vertex,
  Fragment,
  Tessellation
};

struct Image
{
  uint8_t* pixels;
  size_t size = 0;
  uint32_t width = 0;
  uint32_t height = 0;
};

struct ImageView
{

};

struct Sampler
{

};

struct Texture
{
  Image image;
  ImageView* imageView;
  Sampler* sampler;

};


/**
 * This is abstracted in GL, but maps almost 1:1
 * to a vk::DescriptorSetLayoutBinding.
 * It represents one specific resource binding in a shader.
 * We also store an (optional) Buffer/ImageView associated with this binding. 
 * This is useful when later creating the physical descriptor sets, 
 * where we need to know which resources to bind to the descriptors.
 *
 */
struct DescriptorBinding
{
  uint8_t setIndex = 0;
  uint8_t bindingIndex = 0;
  ResourceType type;
  ShaderType shaderType;
  uint32_t count = 1;   // This is useful for arrays/instancing
  Buffer* buffer = nullptr; // This is optional, but useful to store here for later descriptor set creation
  Texture* texture = nullptr; // Optional, needed when image-textures are used:
};


/**
 * This represents a list of layout bindings for shader resources.
 * Schematic only, no real resources are behind this.
 * There may be multile sets used by one pipeline.
 * All the sets used must be defined in the pipelineLayout member of
 * the active PipelineStateObject.
 */
struct DescriptorSetLayout
{
  std::vector<DescriptorBinding*> descriptorBindings;
};

/**
 * This combines a list of DescriptorBindings
 * into 1 set.
 * It is more efficient for modern APIs like
 * Vulkan to handle sets of descriptor bindings at once.
 * These sets are then bound to a pipeline via pipeline layouts.
 */
struct DescriptorSet
{
  DescriptorSetLayout* layout;

};

struct Descriptor
{

  Buffer* buffer;
  DescriptorBinding* binding;

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

struct RenderState
{
  CullMode cullMode = CullMode::Back;
  FillMode fillMode = FillMode::Solid;
  FrontFace frontFace = FrontFace::CounterClockwise;
  bool depthTesting = false;
  bool stencilTesting = false;
  bool blending = false;
  PrimitiveType primitiveType = PrimitiveType::Triangles;
};







/**
 * A single shader - e.g. vertex shader, or fragment shader.
 */
class ShaderModule {
  public:
  virtual void init(ShaderType types, const std::string& source) = 0;
  virtual void* getHandle() = 0;
};

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


/**
 * Represents a combination of different shader modules (or stages).
 * E.g. a vertex->fragmentShader pairing.
 */
class ShaderPipeline {
  public:
      virtual void link(const std::vector<ShaderModule*>& modules) = 0;
      virtual void* getHandle() = 0;
};

struct PipelineLayout
{
  virtual ~PipelineLayout() = default;
  std::vector<tz::DescriptorSetLayout*> descriptorSetLayouts;
};

struct PipelineStateObject
{

  virtual ~PipelineStateObject() = default;
  RenderState renderState;
  ShaderPipeline* shaderPipeline = nullptr;
  VertexLayout vertexLayout;
  std::vector<DescriptorBinding*> descriptorBindings;
  std::vector<DescriptorSetLayout*> descriptorSetLayouts;
  std::vector<DescriptorSet*> descriptorSets;


};


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
      virtual void* getHandle()
      {
        return nullptr;
      }

      virtual void recordCommand(Command* cmd)
      {
        commands.push_back(cmd);
      }

      using iterator = std::vector<Command*>::iterator;
      using const_iterator = std::vector<Command*>::const_iterator;

      virtual iterator begin() { return commands.begin(); }
      virtual iterator end()   { return commands.end(); }

      virtual const_iterator begin() const { return commands.begin(); }
      virtual const_iterator end()   const { return commands.end(); }

  private:
  std::vector<Command*> commands;

};



struct Mesh
{
  std::vector<Eigen::Vector3f> positions;
  std::vector<Eigen::Vector2f> uvs;
  std::vector<Eigen::Vector3f> normals;
  std::vector<uint32_t> indices;
};

struct BitmapData
{
  uint8_t* pixels;
  uint32_t width;
  uint32_t height;
};

BitmapData TZ_API loadBitmapDataFromPath(const std::string & imagePath);


template<typename T>
class TZ_API BufferFactory
{
  virtual Buffer* createBuffer(std::vector<T>& initialData, BufferUsage usage) = 0;
};

class TZ_API Renderer
{
  public:
  virtual void init(tz::Window* window) = 0;
  virtual WindowDesc getRequiredWindowDesc() = 0;
  virtual void clearScreen() = 0;
  virtual void beginDraw(PrimitiveType primitiveType) = 0;
  virtual void endDraw() = 0;
  virtual void emitPosition(Eigen::Vector3f position) = 0;
  virtual void emitColor(Eigen::Vector4f color) = 0;
  virtual void emitUV(Eigen::Vector2f uv) = 0;
  virtual void emitNormal(Eigen::Vector3f normal) = 0;

  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;
  virtual void submitCommandBuffer(CommandBuffer* commandBuffer) = 0;

  virtual Image* createImage(BitmapData bitmapData)
  {
    return nullptr;
  };
  virtual ImageView* createImageView(Image* image)
  {
    return nullptr;
  };
  virtual Sampler* createSampler() { return nullptr; }
  virtual Texture* createTexture(Image* image) { return nullptr;}
  virtual CommandBuffer* createCommandBuffer() = 0;
  virtual Buffer* createBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage) = 0;
  virtual Buffer* createMultiframeBuffer(void* initialData, size_t sizeInBytes, BufferUsage bufferUsage) = 0;
  virtual Buffer* createMultiframeUniformBuffer(uint32_t numberOfPlannedObjects, size_t objectSize) = 0;
  virtual DescriptorBinding* createDescriptorBinding(
    uint8_t binding,
    tz::ResourceType resourceType,
    tz::ShaderType shaderType,
    uint32_t count,
    Buffer* buffer = nullptr,
    Texture* texture = nullptr) = 0;
  virtual DescriptorSetLayout* createDescriptorSetLayout(const std::vector<DescriptorBinding*>& bindings, bool bindless = false) = 0;
  virtual PipelineLayout* createPipelineLayout(std::vector<tz::DescriptorSetLayout*> descriptorSetLayouts) = 0;
  virtual ShaderModule* createShaderModule(ShaderType types, const std::string& source) = 0;
  virtual ShaderPipeline* createShaderPipeline(const std::vector<ShaderModule*>& modules) = 0;
  virtual PipelineStateObject* createPipelineStateObject(RenderState& renderState, ShaderPipeline* shaderPipeline,
                                                         VertexLayout& vertexLayout,  PipelineLayout* providedPipelineLayout) = 0;

  virtual void beginCommandBuffer(CommandBuffer* cb) = 0;

  virtual void endCommandBuffer(CommandBuffer* cb) = 0;

  virtual void recordCommand(CommandBuffer* cb, Command* cmd)
  {
    cb->recordCommand(cmd);
  }

  virtual void updateBuffer(Buffer* buffer, void* data, size_t sizeInBytes, uint32_t objectIndex) = 0;

  virtual tz::DescriptorSet *createMultiframeDescriptorSet(tz::DescriptorSetLayout* descriptorSetLayout)  = 0;

  virtual void updateTextureDescriptorSet(DescriptorSet *pSet, int binding, int index, Texture *pTexture) = 0;
};

enum class VertexShaderType : int
{
  Static,
  Skeletal
};

enum class MaterialType
{
  SingleColor,
  DiffuseNormal,
  PBR,
};

/**
 * Intended use is for selecting PSOs efficiently.
 * Can be used to form a hashkey to select into an
 * unordered map of PSOs.
 *
 */
struct RenderHints
{
  MaterialType materialType = MaterialType::SingleColor;
  VertexShaderType vertexShaderType = VertexShaderType::Static;
  bool wireframe = false;
  bool depthTest = true;
  bool blending = true;
  CullMode cullMode = CullMode::Back;
  uint32_t texture;

  uint64_t getHash() const
  {
    uint64_t key = 0;
    key |= (static_cast<int>(materialType) & 0xFF);
    key |= (static_cast<int>(vertexShaderType) & 0xFF) << 8;
    key |= (wireframe? 1 : 0) << 16;
    key |= (depthTest? 1 : 0) << 17;
    key |= (blending? 1: 0)  << 18;
    key |= (static_cast<int>(cullMode) & 0xFF) << 19;

    return key;


  }
};

struct PrimitiveRenderData
{
  PrimitiveGeometryType geometryType;
  RenderHints renderHints;
  Transform transform;
  Camera* associatedCamera = nullptr;
  Buffer* vertexBuffer = nullptr;
  Buffer* indexBuffer = nullptr;
  uint32_t indexCount = 0;
};


}