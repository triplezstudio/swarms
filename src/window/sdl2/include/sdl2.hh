#pragma once
#include <defines.h>
#include <SDL2/SDL.h>

#include "window_system.hh"
#include "common.hh"


namespace tz {

class TZ_API SDL2WindowSystem : public WindowSystem
{
  public:
  SDL2WindowSystem();
  void init() override;
  void pollEvents() override;
  void present() override;
  Window * createWindow(WindowDesc desc) override;

  GraphicsSurface createSurface(GraphicsInstance& instance, WindowDesc desc) override;

  [[nodiscard]] const std::vector<SDL_Event>& getFrameEvents() const;

  private:
  client_common::NativeHandles getNativeHandles();


  private:
  SDL_Window* window = nullptr;
  WindowDesc windowDesc;
  std::vector<SDL_Event> frameEvents;

};

}


