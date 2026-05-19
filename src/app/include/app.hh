#include <Eigen/Dense>
#include <defines.h>
#include <functional>
#include <string>
#include <text_render.hh>
#include <vulkan_renderer.hh>
#include <window.hh>
#include <input.hh>
#include <render_helpers.hh>
#include <scene_helper.hh>

namespace tz {
namespace rv =  render::vulkan;


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









  class App;
  using FrameListener = std::function<void(App* app)>;
  using InputListener = std::function<void(const tz::input::SDL2InputSystem& inputSystem)>;

  class TZ_API App
  {


    public:
      App(int width, int height, const std::string& title);
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

      void addScene(const std::string& name, Scene& scene);

  private:
      tz::input::SDL2InputSystem& inputSystem;
      rv::Renderer* renderer = nullptr;
      tz::render::TextRenderer* textRenderer = nullptr;

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
      std::map<std::string, tz::render::TextGeometry> textGeometries;
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
      void createMasterPipelineLayout();
      void renderPrimitives(const std::vector<PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);

      void updateInputListeners();
      Window *window = nullptr;

      std::map<std::string, Scene> scenes;
  };


}

