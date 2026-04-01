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

class SWARMS_API SDL2App : public app_base::AppImpl
{
  public:
  void init() override;
  void run() override;


  private:
  int decideRenderBackend();
  client_common::NativeHandles getNativeHandles();


  void initVulkanApp();
  void initOpenGLApp();



  private:
  SDL_Window* window = nullptr;
};

}


