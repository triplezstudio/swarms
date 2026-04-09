#pragma once

#include <Eigen/Dense>
#include <window_system.hh>


/**
 * The render interface.
 * It abstracts over any underlying render implementation, e.g. custom Vulkan, OpenGL
 * or any 3rd party render library.
 * In any case we want to be able to
 * a) switch render backends in the future if specific features are needed
 * b) be able to provide platform optimal render backends so we need to be able to support
 *    multiple renderers behind this interface anyway.
 */
namespace tz {

struct Buffer
{
  virtual void updateData(uint64_t sizeInBytes, void* data)= 0;
  virtual void* getHandle() = 0;
};

struct VertexBuffer
{
  virtual void* getHandle() = 0;

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



struct DescriptorBinding
{
  uint8_t set = 0;
  uint8_t binding = 0;
  ResourceType type;
};

struct Descriptor
{
  Buffer* buffer;
  DescriptorBinding binding;

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
  CullMode cullMode;
  FillMode fillMode;
  FrontFace frontFace;
  bool depthTesting;
  bool stencilTesting;
  bool blending;
  PrimitiveType primitiveType;
};





enum class ShaderType {
  Vertex,
  Fragment,
  Tessellation
};

/**
 * A single shader - e.g. vertex shader, or fragment shader.
 */
class ShaderModule {
  public:
  virtual void init(ShaderType type, const std::string& source) = 0;
  virtual void* getHandle() = 0;
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

struct PipelineStateObject
{
  RenderState renderState;
  ShaderPipeline* shaderPipeline = nullptr;
  VertexLayout vertexLayout;
  std::vector<DescriptorBinding> descriptorBindings;

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
      CmdBindDescriptors(std::vector<Descriptor> descs) : descriptors(descs)
      {

      }

      std::vector<Descriptor> descriptors;
};

class CmdBindVertexBuffers : public Command
{
  public:
      CmdBindVertexBuffers(std::vector<VertexBuffer*> vbs) : vertexBuffers(vbs) {}


  std::vector<VertexBuffer*> vertexBuffers;
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
      void recordCommand(Command* cmd)
      {
        commands.push_back(cmd);
      }

      using iterator = std::vector<Command*>::iterator;
      using const_iterator = std::vector<Command*>::const_iterator;

      iterator begin() { return commands.begin(); }
      iterator end()   { return commands.end(); }

      const_iterator begin() const { return commands.begin(); }
      const_iterator end()   const { return commands.end(); }

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

class Renderer
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

  virtual VertexBuffer* createVertexBuffer(const std::vector<Eigen::Vector3f>& data) = 0;
  virtual Buffer* createBuffer(uint64_t sizeInBytes, void* data) = 0;

};

}