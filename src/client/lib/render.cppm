module;
#include <Eigen/Dense>
export module render.base;

import windowing;
import common;

/**
 * The render interface.
 * It abstracts over any underlying render implementation, e.g. custom Vulkan, OpenGL
 * or any 3rd party render library.
 * In any case we want to be able to
 * a) switch render backends in the future if specific features are needed
 * b) be able to provide platform optimal render backends so we need to be able to support
 *    multiple renderers behind this interface anyway.
 */
export namespace tz {

struct VertexBuffer
{
  void* handle;

};

enum class VertexInputRate
{
  PerVertex,
  PerInstance
};

enum class AttributeDataFormat
{
  R32G32B32A32_FLOAT,
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
  AttributeDataFormat dataFormat;
  uint32_t offset;


};

struct VertexLayout
{
  std::vector<VertexBinding> bindings;
  std::vector<VertexAttribute> attributes;
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
      virtual void link(std::vector<ShaderModule*> modules) = 0;
      virtual void* getHandle() = 0;
};

struct PipelineStateObject
{
  RenderState renderState;
  ShaderPipeline* shaderPipeline = nullptr;
  VertexLayout vertexLayout;
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



};

}