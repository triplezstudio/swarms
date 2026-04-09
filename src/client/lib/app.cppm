#include <defines.h>
#include <functional>

#include "window_system.cppm"
#include "render.cppm"

namespace tz {

  using FrameListener = std::function< void(float frameTime)>;

  class SWARMS_API App
  {
    public:
      App(tz::WindowSystem* windowSystem, tz::Renderer* renderer);
      virtual void run();
      virtual void addFrameListener(FrameListener frameListener);

  private:
      tz::WindowSystem* windowSystem = nullptr;
      tz::Renderer* renderer = nullptr;

      std::vector<FrameListener> frameListeners;

      void updateFrameListeners(float frameTime);


  };


}

