module;
#include <defines.h>
#include <functional>
export module app;

import :base;
import render.base;

export namespace app {

  using FrameListener = std::function< void(float frameTime)>;

  class SWARMS_API App
  {
    public:
      App();
      virtual void init();
      virtual void run();
      virtual void addFrameListener(FrameListener frameListener);
      render::Renderer* getRenderer();

  private:
      app_base::AppImpl* impl = nullptr;
      std::vector<FrameListener> frameListeners;

      void updateFrameListeners(float frameTime);


  };


}

