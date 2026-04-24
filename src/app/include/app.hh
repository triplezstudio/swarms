#include <defines.h>
#include <functional>

#include <window_system.hh>
#include <render.hh>

namespace tz {

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
    PrimitiveMaterialType materialType;
    Eigen::Vector3f position;
    Eigen::Vector3f scale;
    Eigen::Vector3f color;
    Eigen::Quaternionf orientation;
    Camera* associatedCamera = nullptr;
  };

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

      virtual void renderColoredQuad(Eigen::Vector3f position, Eigen::Vector3f scale = {1, 1,1}, Eigen::Vector3f color = {1, 1,1});
      virtual void renderCube(Eigen::Vector3f position);


  private:
      WindowSystem* windowSystem = nullptr;
      Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

      std::vector<uint32_t> quadIndices;
      Buffer* quadVertexBuffer = nullptr;
      Buffer* quadIndexBuffer = nullptr;
      PipelineStateObject* colorOnlyPSO = nullptr;
      CommandBuffer* commandBuffer = nullptr;
      std::vector<PrimitiveRenderData> framePrimitives;

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
  };


}

