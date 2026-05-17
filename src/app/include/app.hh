#include <Eigen/Dense>
#include <defines.h>
#include <functional>
#include <string>
#include <text_render.hh>
#include <render_helpers.hh>
#include <vulkan_renderer.hh>
#include <window_system.hh>
#include <input.hh>
#include <scene.hh>

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
      App();
      virtual void run();
      virtual void setUpdateFunction(FrameListener frameListener);
      virtual float getLastFrameTime();

      virtual void activate3DCamera();
      virtual void activate3DCamera(Eigen::Vector3f position, Eigen::Vector3f lookAt);
      virtual void activateUICamera();
      virtual void activateUICamera(Eigen::Vector3f position);

      virtual void renderQuad(tz::scene::Transform transform, tz::scene::RenderHints renderHints = {});
      virtual void renderCube(tz::scene::Transform transform, tz::scene::RenderHints renderHints = {});
      virtual void renderSphere(tz::scene::Transform transform, tz::scene::RenderHints renderHints = {});
      virtual void renderCylinder(tz::scene::Transform transform, tz::scene::RenderHints renderHints = {});

      uint32_t createTexture(const std::string& imagePath);

      virtual tz::render::helpers::TextRenderHelper& getTextRenderHelper();

  private:
      WindowSystem* windowSystem = nullptr;
      tz::input::SDL2InputSystem& inputSystem;
      rv::Renderer* renderer = nullptr;


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

      uint32_t tzLabelIndexCount = 0;
      rv::PipelineStateObject* colorOnlyPSO = nullptr;
      rv::CommandBuffer* commandBuffer = nullptr;


      rv::DescriptorSet* cameraDescriptorSet = nullptr;
      rv::DescriptorSet* perObjectDescriptorSet = nullptr;
      rv::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
      rv::PipelineLayout* masterPipelineLayout = nullptr;

      uint32_t globalTextureIndex = 0;

      tz::scene::Camera* default3DCamera = nullptr;
      tz::scene::Camera* defaultUICamera = nullptr;
      tz::scene::Camera* activeRenderCamera = nullptr;

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
      std::vector<tz::scene::PrimitiveRenderData> getRenderPrimitivesByCamera(tz::scene::Camera *camera);
      render::vulkan::Renderer *vulkanRenderer();
      void createMasterPipelineLayout();
      void renderPrimitives(const std::vector<tz::scene::PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);

      void updateInputListeners();

      tz::render::helpers::TextRenderHelper* textRenderHelper = nullptr;
  };


}

