module;
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <iostream>
#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_WAYLAND_KHR
#endif
#include <vulkan/vulkan_raii.hpp>
#include <SDL_syswm.h>
#include <vulkan/vulkan_win32.h>
#include <SDL2/SDL.h>

module render.vulkan;

import common;

render::vulkan::VulkanRenderer::VulkanRenderer(std::vector<const char*> extensionNames) : extensions(extensionNames)
{

}


void render::vulkan::VulkanRenderer::draw()
{

}

void render::vulkan::VulkanRenderer::initSurface() {
#ifdef _WIN32

  VkWin32SurfaceCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  auto nativeHandles = handleProvider->getNativeHandles();
  createInfo.hwnd = (HWND)nativeHandles.window;
  createInfo.hinstance =(HINSTANCE) nativeHandles.connection;

//  if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
//    throw std::runtime_error("failed to create window surface!");
//  }
#endif
}

void render::vulkan::VulkanRenderer::init(client_common::NativeHandleProvider* handleProvider)
{

  this->handleProvider = handleProvider;
  constexpr vk::ApplicationInfo appInfo{.pApplicationName   = "swarms",
                                        .applicationVersion = VK_MAKE_VERSION( 0, 0, 1 ),
                                        .pEngineName        = "triplez",
                                        .engineVersion      = VK_MAKE_VERSION( 0, 0, 1 ),
                                        .apiVersion         = vk::ApiVersion12};


  auto availableExtensions = context.enumerateInstanceExtensionProperties();

  std::cout << "Available extensions:\n";
  for (const auto& ext : availableExtensions) {
    std::cout << "\t" << ext.extensionName << "\n";
  }


  vk::InstanceCreateInfo createInfo = {};
  createInfo.setPApplicationInfo(&appInfo);
  createInfo.setPEnabledExtensionNames(extensions);

  instance = vk::raii::Instance(context, createInfo);

  std::cout << "Vulkan instance created" << std::endl;

}