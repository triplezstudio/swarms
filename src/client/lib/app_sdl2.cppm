//
// Created by mgrus on 28.03.2026.
//
module;
#include <defines.h>
#include <SDL2/sdl.h>
export module app:sdl2;

import :base;
import common;


namespace sdl2 {

class SWARMS_API SDL2App : public app_base::AppImpl, client_common::NativeHandleProvider
{
  public:
    void init() override;
    void run() override;
    client_common::NativeHandles getNativeHandles() override;

  private:
      int decideRenderBackend();

  private:
    SDL_Window* window = nullptr;
};

}


