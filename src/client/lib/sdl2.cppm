//
// Created by mgrus on 28.03.2026.
//
module;
#include <defines.h>
#include <SDL2/sdl.h>
export module windowing.sdl2;

import windowing;
import common;

export namespace tz {

class SDL2WindowSystem : public WindowSystem
{
  public:
  void init() override;
  Window * createWindow(tz::WindowDesc desc) override;

  private:
  client_common::NativeHandles getNativeHandles();



  private:
  SDL_Window* window = nullptr;
};

}


