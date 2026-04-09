#pragma once
#include <defines.h>
#include <SDL2/SDL.h>

#include "window_system.hh"
#include "common.hh"


namespace tz {

class SWARMS_API SDL2WindowSystem : public WindowSystem
{
  public:
  SDL2WindowSystem();
  void init() override;
  void pollEvents() override;
  void present() override;
  Window * createWindow(WindowDesc desc) override;

  private:
  client_common::NativeHandles getNativeHandles();



  private:
  SDL_Window* window = nullptr;
  WindowDesc windowDesc;
};

}


