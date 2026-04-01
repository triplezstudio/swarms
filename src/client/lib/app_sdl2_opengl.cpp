module;

#include <GL/glew.h>
#include <SDL2/SDL_video.h>
#include <Eigen/Dense>

#include <iostream>
#include <vector>

#include <stdexcept>

module app;


import :sdl2;

import render.opengl;



namespace sdl2 {



void SDL2App::initOpenGLApp()
{
  auto windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  window = SDL_CreateWindow("swarms v0.0.1 (opengl 4.6)",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                            windowFlags);

  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Initialize GLEW
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    // GLEW initialization failed
    fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
    exit(1);
  }

  render::OpenGLInitData initData;
  initData.displaySizeFunc =  [this](int* width, int* height) {
    SDL_GL_GetDrawableSize(this->window, width, height);
  };
  renderer = new render::OpenGLRenderer(initData);


}

void SDL2App::doGLFrame()
{

  dynamic_cast<render::OpenGLRenderer*>(renderer)->endFrame();
  SDL_GL_SwapWindow(window);
  dynamic_cast<render::OpenGLRenderer*>(renderer)->beginFrame();

}
}