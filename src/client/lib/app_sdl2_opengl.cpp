module;

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL_syswm.h>

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

  // We expect to have received all render commands at this stage.
  // Our task is now to render as efficient as possible.
  static float r = 0;
  r+= 0.001;
  glClearColor(sinf(r), 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);
}
}