//
// Created by mgrus on 28.03.2026.
//
module;
#include <defines.h>
#include <SDL2/sdl.h>
export module app:sdl2;

import :base;
import common;

import render.base;


namespace sdl2 {

class SWARMS_API SDL2App : public app_base::AppImpl
{
  public:
  void init() override;
  void doFrame() override;
  render::Renderer* getRenderer() override;


  private:
  int decideRenderBackend();
  client_common::NativeHandles getNativeHandles();


  void initVulkanApp();
  void initOpenGLApp();

  void doGLFrame();
  void doVulkanFrame();




  private:
  SDL_Window* window = nullptr;
  int renderBackendFlag = 0;
  render::Renderer* renderer;
};

}


