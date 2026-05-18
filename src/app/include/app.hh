#include <defines.h>
#include <functional>
#include <input.hh>
#include <render.hh>
#include <string>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>

namespace tz {
namespace rv =  render::vulkan;



struct alignas(16) TransformUniformBufferObject
{
  Eigen::Matrix4f model;

};



struct Transform
{
  Eigen::Vector3f position = {0, 0, 0};
  Eigen::Vector3f scale = {1, 1, 1};
  Eigen::Quaternionf orientation;
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
  Text,
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
  rv::CullMode cullMode = rv::CullMode::Back;
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


struct PrimitiveRenderData
{
  PrimitiveGeometryType geometryType;
  RenderHints renderHints;
  Transform transform;
  Camera* associatedCamera = nullptr;
  rv::Buffer* vertexBuffer = nullptr;
  rv::Buffer* indexBuffer = nullptr;
  uint32_t indexCount = 0;
};





  class App;
  using FrameListener = std::function<void(App* app)>;
  using InputListener = std::function<void(const tz::input::SDL2InputSystem& inputSystem)>;

  class TZ_API App
  {

    public:
      App();
      virtual void run();
      virtual void setUpdateFunction(FrameListener frameListener);
      virtual void setInputListenerFunc(InputListener inputListener);
      virtual float getLastFrameTime();

      virtual void activate3DCamera();
      virtual void activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt);
      virtual void activateUICamera();
      virtual void activateUICamera(Eigen::Vector3f position);

      virtual void renderQuad(Transform transform, RenderHints renderHints = {});
      virtual void renderCube(Transform transform, RenderHints renderHints = {});
      virtual void renderSphere(Transform transform, RenderHints renderHints = {});
      virtual void renderCylinder(Transform transform, RenderHints renderHints = {});

      uint32_t createTexture(const std::string& imagePath);

      int createFont(const std::string& fileName, int size);
      void renderText(Transform transform, const std::string& text, int fontId = -1);

  private:
      WindowSystem* windowSystem = nullptr;
      tz::input::SDL2InputSystem& inputSystem;
      rv::Renderer* renderer = nullptr;
      tz::text::TextRenderer* textRenderer = nullptr;

      std::vector<FrameListener> frameListeners;
      std::vector<InputListener> inputListeners;

      std::vector<uint32_t> quadIndices;
      std::vector<uint32_t> cubeIndices;
      std::vector<uint32_t> cubeIndicesPosTex;
      rv::Buffer* quadPosVertexBuffer       = nullptr;
      rv::Buffer* cubePosVertexBuffer = nullptr;
      rv::Buffer* cubePosTexCoordVertexBuffer = nullptr;
      rv::Buffer* quadPosTexCoordVertexBuffer = nullptr;
      rv::Buffer* quadIndexBuffer = nullptr;
      rv::Buffer* cubeIndexBuffer = nullptr;
      rv::Buffer* cubeTexIndexBuffer = nullptr;
      rv::Buffer* tzLabelVertexBuffer = nullptr;
      rv::Buffer* tzLabelIndexBuffer = nullptr;
      std::map<std::string, tz::text::TextGeometry> textGeometries;
      std::map<std::string, rv::Buffer*> textVertexBuffers;
      std::map<std::string, rv::Buffer*> textIndexBuffers;
      std::map<int, uint32_t> fontTextureMap;
      uint32_t tzLabelIndexCount = 0;
      rv::PipelineStateObject* colorOnlyPSO = nullptr;
      rv::CommandBuffer* commandBuffer = nullptr;
      std::vector<PrimitiveRenderData> framePrimitives;

      rv::DescriptorSet* cameraDescriptorSet = nullptr;
      rv::DescriptorSet* perObjectDescriptorSet = nullptr;
      rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
      rv::PipelineLayout* masterPipelineLayout = nullptr;

      uint32_t globalTextureIndex = 0;

      Camera* default3DCamera = nullptr;
      Camera* defaultUICamera = nullptr;
      Camera* activeRenderCamera = nullptr;

      void updateFrameListeners(float frameTime);
      void prepareRenderPrimitives();
      rv::PipelineStateObject* createColorOnlyPSO();
      Eigen::Matrix4f createPerspectiveProjectionMatrix(float fovY,
                                                        float aspect,
                                                        float zNear,
                                                        float zFar);
      Eigen::Matrix4f createLookAtMatrix(const Eigen::Vector3f &eye,
                                         const Eigen::Vector3f &center,
                                         const Eigen::Vector3f &up);

      std::unordered_map<uint64_t, rv::PipelineStateObject*> psoCache;
      void buildPSOCache();
      rv::PipelineStateObject *createTexturedPSO();
      rv::PipelineStateObject *createTextPSO();
      void renderFrame();
      std::vector<PrimitiveRenderData> getRenderPrimitivesByCamera(Camera *camera);
      render::vulkan::Renderer *vulkanRenderer();
      void renderPrimitives(const std::vector<PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);
      int uiFont = -1;
      uint32_t uiFontAtlasTextureIndex = 0;
      void updateInputListeners();
  };


}

