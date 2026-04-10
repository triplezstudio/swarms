#include <defines.h>
#include <functional>

#include <window_system.hh>
#include <render.hh>

namespace tz {

  using FrameListener = std::function< void(float frameTime)>;

  class TZ_API App
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

