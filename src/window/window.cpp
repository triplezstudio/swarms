#include <SDL3/SDL.h>

#include <vector>

#include <SDL3/SDL_vulkan.h>
#include <common.hh>
#include <iostream>
#include <stdexcept>
#include <window.hh>

namespace tz {

static bool isInputEvent(SDL_Event event)
{
  return event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP
         || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || event.type == SDL_EVENT_MOUSE_BUTTON_UP;
}

void Window::pollEvents()
{
  frameInputEvents.clear();
  SDL_Event event;
  while (SDL_PollEvent(&event))
  {
    if (isInputEvent(event))
    {
      frameInputEvents.push_back(event);
    }

    if (event.type == SDL_EVENT_QUIT)
    {
      // TODO handle graceful shutdown
      exit(0);
    }
  }
}

tz::Window::Window(int width, int height, const std::string &title) : width(width), height(height)
{
  if (SDL_Init(SDL_INIT_EVENTS) < 0)
  {
    throw std::runtime_error("SDL could not initialize! SDL_Error: " + std::string(SDL_GetError()));
  }

  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  SDL_Event event;
  bool run = true;

  int windowFlags = SDL_WINDOW_VULKAN;


  _window = SDL_CreateWindow(title.c_str(), width, height, windowFlags);
}

VkSurfaceKHR Window::createSurface(VkInstance instance)
{
  VkSurfaceKHR rawSurface;
  if (!SDL_Vulkan_CreateSurface(_window, instance, nullptr, &rawSurface))
  {
    std::cerr << "surface error: " << std::string(SDL_GetError()) << std::endl;
    throw std::runtime_error("Could not create surface!" + std::string(SDL_GetError()));
  }
  return rawSurface;
}

void Window::getDisplaySize(int &width, int &height)
{
  SDL_GetWindowSizeInPixels(_window, &width, &height);
}

} // namespace tz
