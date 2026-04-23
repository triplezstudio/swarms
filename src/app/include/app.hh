#include <defines.h>
#include <functional>

#include <window_system.hh>
#include <render.hh>

namespace tz {

  enum class PrimitiveRenderType
  {
    Line,
    Quad,
    Cube,
    Sphere,
    Mesh
  };

  struct PrimitiveRenderData
  {
    PrimitiveRenderType type;
    Eigen::Vector3f position;
    Eigen::Vector3f scale;
    Eigen::Quaternionf orientation;
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

      virtual void renderColoredQuad(Eigen::Vector3f position);
      virtual void renderCube(Eigen::Vector3f position);


  private:
      WindowSystem* windowSystem = nullptr;
      Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

      PipelineStateObject* colorOnlyPSO = nullptr;
      CommandBuffer* commandBuffer = nullptr;
      std::vector<PrimitiveRenderData> framePrimitives;


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

