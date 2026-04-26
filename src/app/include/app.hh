#include <defines.h>
#include <functional>

#include <window_system.hh>
#include <render.hh>
#include <vulkan_renderer.hh>


namespace tz {



  class App;
  using FrameListener = std::function< void(App* app)>;

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

      virtual void renderQuad(Transform transform, RenderHints renderHints = {});
      virtual void renderCube(Transform transform, RenderHints renderHints = {});
      virtual void renderSphere(Transform transform, RenderHints renderHints = {});
      virtual void renderCylinder(Transform transform, RenderHints renderHints = {});

      uint32_t createTexture(const std::string& imagePath);

  private:
      WindowSystem* windowSystem = nullptr;
      Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

      std::vector<uint32_t> quadIndices;
      Buffer* quadPosVertexBuffer       = nullptr;
      Buffer* quadPosTexCoordVertexBuffer = nullptr;
      Buffer* quadIndexBuffer = nullptr;
      PipelineStateObject* colorOnlyPSO = nullptr;
      CommandBuffer* commandBuffer = nullptr;
      std::vector<PrimitiveRenderData> framePrimitives;

      tz::DescriptorSet* cameraDescriptorSet = nullptr;
      tz::DescriptorSet* transformDescriptorSet = nullptr;
      tz::DescriptorSet* perObjectDescriptorSet = nullptr;
      tz::DescriptorSet* diffuseTextureDescriptorSet = nullptr;
      tz::PipelineLayout* masterPipelineLayout = nullptr;

      uint32_t globalTextureIndex = 0;

      Camera* default3DCamera = nullptr;
      Camera* defaultUICamera = nullptr;
      Camera* activeRenderCamera = nullptr;

      void updateFrameListeners(float frameTime);
      void prepareRenderPrimitives();
      PipelineStateObject* createColorOnlyPSO();
      Eigen::Matrix4f createPerspectiveProjectionMatrix(float fovY,
                                                        float aspect,
                                                        float zNear,
                                                        float zFar);
      Eigen::Matrix4f createLookAtMatrix(const Eigen::Vector3f &eye,
                                         const Eigen::Vector3f &center,
                                         const Eigen::Vector3f &up);

      std::unordered_map<uint64_t, PipelineStateObject*> psoCache;
      void buildPSOCache();
      PipelineStateObject *createTexturedPSO();
      void renderFrame();
      std::vector<PrimitiveRenderData> getRenderPrimitivesByCamera(Camera *camera);
      render::vulkan::VulkanRenderer *vulkanRenderer();
      void createMasterPipelineLayout();
      void renderPrimitives(const std::vector<PrimitiveRenderData> &primitives,
                            uint32_t &primitiveCounter);
  };


}

