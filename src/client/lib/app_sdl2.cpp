module;
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_TYPESAFE_CONVERSION 1

#include <vulkan/vulkan_raii.hpp>

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
  // For now we just set vulkan as the default
  return SDL_WINDOW_VULKAN;
}


void SDL2App::init()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
  }


  auto vkLoad = SDL_Vulkan_LoadLibrary("vulkan-1.dll");
  if (vkLoad != 0) {
    std::cerr << "sdl2 vulkan not loading!" << SDL_GetError() << std::endl;
  }

  VULKAN_HPP_DEFAULT_DISPATCHER.init(
    reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr())
  );

  auto renderBackendFlag = decideRenderBackend();
  auto windowFlags = SDL_WINDOW_SHOWN | renderBackendFlag;

  // Create a Window
  window = SDL_CreateWindow("swarms v0.0.1",
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

  if (renderBackendFlag == SDL_WINDOW_VULKAN) {
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
    auto vkRenderer = render::VulkanRenderer(vulkanInitData);
    vkRenderer.init();
  }
  // TODO: implement branches for other render backends.

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


