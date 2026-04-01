module;
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_TYPESAFE_CONVERSION 1

#include <vulkan/vulkan_raii.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
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

void SDL2App::initVulkanApp()
{
  VULKAN_HPP_DEFAULT_DISPATCHER.init(
    reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr()));

  auto windowFlags = SDL_WINDOW_SHOWN | renderBackendFlag;

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
  if (!SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, nullptr))
  {
    throw std::runtime_error("SDL could not get Vulkan extensions: " + std::string(SDL_GetError()));
  }
  std::vector<const char *> extensions(sdlExtensionCount);
  SDL_Vulkan_GetInstanceExtensions(window, &sdlExtensionCount, extensions.data());

  render::VulkanInitData vulkanInitData;
  vulkanInitData.nativeHandles       = getNativeHandles();
  vulkanInitData.extensions          = extensions;
  vulkanInitData.surfaceCreationFunc = [this](vk::raii::Instance &inst) -> vk::raii::SurfaceKHR {
    return createVulkanSurface(this->window, inst);
  };
  vulkanInitData.displaySizeFunc = [this](int *width, int *height) {
    SDL_Vulkan_GetDrawableSize(this->window, width, height);
  };

  // TODO vulkan renderer is work-in-progress
  //auto vkRenderer = render::VulkanRenderer(vulkanInitData);
  //vkRenderer.init();
}


vk::raii::SurfaceKHR createVulkanSurface(SDL_Window* window, vk::raii::Instance& instance)
{
  VkSurfaceKHR rawSurface;
  if (!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance> (*instance), &rawSurface))
  {
    throw std::runtime_error("Could not create surface!" + std::string(SDL_GetError()));
  }

  return vk::raii::SurfaceKHR(instance, rawSurface);
}

}