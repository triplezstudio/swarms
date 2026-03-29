module;
#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL_syswm.h>
#include <stdexcept>
#include <SDL_vulkan.h>

module app;

import :sdl2;

import render.vulkan;


namespace sdl2 {

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
  // 1. Try to load it normally
  HMODULE handle = LoadLibraryA("vulkan-1.dll");
  if (!handle) {
    DWORD err = GetLastError();
    std::cout << "[FAILURE] LoadLibrary failed. Error code: " << err << std::endl;

    if (err == ERROR_MOD_NOT_FOUND) {
      std::cout << "-> Reason: Windows literally cannot find the file. It's not in System32 or your PATH." << std::endl;
    } else if (err == ERROR_BAD_EXE_FORMAT) {
      std::cout << "-> Reason: The DLL is 32-bit and your app is 64-bit (or vice versa)." << std::endl;
    } else if (err == ERROR_DLL_INIT_FAILED) {
      std::cout << "-> Reason: The DLL found its dependencies but failed to initialize (Driver issue)." << std::endl;
    } else {
      // Error 126 is common if a SUB-dependency of vulkan-1.dll is missing
      std::cout << "-> Reason: Likely a missing dependency (e.g., your GPU driver is partially uninstalled)." << std::endl;
    }
  }

  auto vkLoad = SDL_Vulkan_LoadLibrary("vulkan-1.dll");
  if (vkLoad != 0) {
    std::cerr << "sdl2 vulkan still not loading!" << SDL_GetError() << std::endl;
  }

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
    std::vector<const char*> extensionNames(sdlExtensionCount);
    SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, extensionNames.data());

    auto vkRenderer = render::vulkan::VulkanRenderer(extensionNames);
    vkRenderer.init(this);
  }
  // TODO: implement branches for other render backends.

  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  // Cleanup
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void SDL2App::run() {}

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


