#pragma once
#include "defines.h"
#include "common.hh"
#include <SDL2/SDL.h>
#include <SDL_vulkan.h>
#include <functional>
#include <string>

namespace tz {

enum class KeyCode
{
  NONE,
  A,
  B,
  C,
  D,
  E,
  F,
  G,
  H,
  I,
  J,
  K,
  L,
  M,
  N,
  O,
  P,
  Q,
  R,
  S,
  T,
  U,
  V,
  W,
  X,
  Y,
  Z,
  ENTER,
  ESC,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  CTRL_LEFT,
  CTRL_RIGHT,
  SHIFT_LEFT,
  SHIFT_RIGHT,
  NUM_1,
  NUM_2,
  NUM_3,
  NUM_4,
  NUM_5,
  NUM_6,
  NUM_7,
  NUM_8,
  NUM_9,
  NUM_0,
  F1,
  F2,
  F3,
  F4,
  F5,
  F6,
  F7,
  F8,
  F9,
  F10,
  F11,
  F12


};

enum class MouseButton
{
  LEFT,
  RIGHT,
  MIDDLE,
  NONE
};


struct GraphicsSurface {
  void* handle;
};

struct GraphicsInstance
{
  void* handle;
};


struct WindowDesc {
  int width;
  int height;
  std::string title;

};

enum class FrameInputType
{
  KeyDown,
  KeyWasPressed,
  MouseButtonWasPressed,
  MouseButtonDown,
};

struct FrameInputEvent
{
  FrameInputType frameInputType;
  KeyCode keyCode;
  MouseButton mouseButton;

};

struct TZ_API Window
{
  Window(int width, int height, const std::string& title);
  void pollEvents();
  void getDisplaySize(int& width, int& height);
  VkSurfaceKHR createSurface(VkInstance vkInstance);

  SDL_Window* _window = nullptr;
  std::vector<SDL_Event> frameInputEvents;
  void* nativeHandle = nullptr;
  int width;
  int height;
  const std::string title;
};

}



