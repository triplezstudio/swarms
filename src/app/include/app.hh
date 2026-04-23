#include <defines.h>
#include <functional>

#include <window_system.hh>
#include <render.hh>

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

      virtual void renderCube(Eigen::Vector3f position);


  private:
      tz::WindowSystem* windowSystem = nullptr;
      tz::Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

      void updateFrameListeners(float frameTime);


  };


}

