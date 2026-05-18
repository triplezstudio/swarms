#pragma once
#include <../../../client_common/include/defines.h>
#include <SDL2/SDL.h>

#include "../../../client_common/include/common.hh"
#include "window_system.hh"

namespace tz {

class SDLFrameEvent
{

  SDL_Event event;
};

class TZ_API SDL2WindowSystem : public WindowSystem
{
  public:
  SDL2WindowSystem();
  void init() override;
  void pollEvents() override;
  void present() override;
  Window * createWindow(WindowDesc desc) override;

  GraphicsSurface createSurface(GraphicsInstance& instance) override;

  [[nodiscard]] const std::vector<SDL_Event> getFrameEvents() const;

  private:
  NativeHandles getNativeHandles();


  private:
  SDL_Window* window = nullptr;
  WindowDesc windowDesc;
  std::vector<SDL_Event> frameInputEvents;

};

}


