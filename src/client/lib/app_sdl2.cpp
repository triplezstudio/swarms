module;
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_TYPESAFE_CONVERSION 1

#include <vulkan/vulkan_raii.hpp>
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

#include <iostream>
#include <vector>

#include <stdexcept>



module app;


import :sdl2;

import render.vulkan;



namespace sdl2 {

vk::raii::SurfaceKHR createVulkanSurface(SDL_Window* window, vk::raii::Instance& instance);

int SDL2App::decideRenderBackend() {
  // TODO: read configfile and or env var etc.
  // For now we just set PpenGL as the default
  return SDL_WINDOW_OPENGL;
}

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
  if (GLEW_OK != err) {
    // GLEW initialization failed
    fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
    exit(1);
  }

  glClearColor(0.5, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);


}

void SDL2App::initVulkanApp()
{
  VULKAN_HPP_DEFAULT_DISPATCHER.init(
    reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr())
  );


  auto windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN;

  // Create a Window
  window = SDL_CreateWindow("swarms v0.0.1 (vulkan 1.3)",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640,
                            480,
                            windowFlags);

  if (!window)
  {
    auto err = std::string(SDL_GetError());
    throw std::runtime_error("Window could not be created! SDL_Error: " + err);
  }

  uint32_t sdlExtensionCount = 0;
  if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr)) {
    throw std::runtime_error("SDL could not get Vulkan extensions: " + std::string(SDL_GetError()));
  }
  std::vector<const char*> extensions(sdlExtensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, extensions.data());

  render::VulkanInitData vulkanInitData;
  vulkanInitData.nativeHandles = getNativeHandles();
  vulkanInitData.extensions = extensions;
  vulkanInitData.surfaceCreationFunc = [this](vk::raii::Instance& inst) -> vk::raii::SurfaceKHR {
    return createVulkanSurface(this->window,inst);
  };
  vulkanInitData.displaySizeFunc = [this](int* width, int* height) {
    SDL_Vulkan_GetDrawableSize(this->window, width, height);
  };

  // TODO vulkan renderer is work-in-progress
  //auto vkRenderer = render::VulkanRenderer(vulkanInitData);
  //vkRenderer.init();

}

void SDL2App::init()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
  }

  auto renderBackendFlag = decideRenderBackend();

  switch (renderBackendFlag) {
    case SDL_WINDOW_VULKAN: initVulkanApp(); break;
    case SDL_WINDOW_OPENGL: initOpenGLApp(); break;
  }

  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  // Cleanup
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void SDL2App::run() {}


vk::raii::SurfaceKHR createVulkanSurface(SDL_Window* window, vk::raii::Instance& instance)
{
    VkSurfaceKHR rawSurface;
    if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance> (*instance), &rawSurface))
    {
      throw std::runtime_error("Could not create surface!" + std::string(SDL_GetError()));
    }

    return vk::raii::SurfaceKHR(instance, rawSurface);
}

client_common::NativeHandles SDL2App::getNativeHandles()
{
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version); // Initialize version info

  if (SDL_GetWindowWMInfo(window, &wmInfo))
  {
#if defined(_WIN32)
    // Windows: connection is HINSTANCE, window is HWND
    return {(void *) wmInfo.info.win.hinstance, (void *) wmInfo.info.win.window, "windows"};

#elif defined(__linux__)
    // Linux: Check if we are running under X11 or Wayland
    if (wmInfo.subsystem == SDL_SYSWM_X11)
    {
      return {(void *) wmInfo.info.x11.display, (void *) (uintptr_t) wmInfo.info.x11.window, "x11"};
    }
    else if (wmInfo.subsystem == SDL_SYSWM_WAYLAND)
    {
      return {(void *) wmInfo.info.wl.display, (void *) wmInfo.info.wl.surface, "wayland"};
    }
#endif
  }

  return {nullptr, nullptr};
}

}


