#pragma once
#include <defines.h>
#include <sdl2.hh>

namespace tz::input {
enum class KeyCode
{
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

class TZ_API SDL2InputSystem
{
public:
  SDL2InputSystem(const SDL2WindowSystem& windowSystem);

  // For single key presses, e.g. menu
  bool isKeyPressed(KeyCode keyCode);

  // For continuous tracking, e.g. WASD movement
  bool isKeyDown(KeyCode keyCode);

  private:

      KeyCode fromSDLEvent(SDL_Event ev);
      SDL_Scancode toSDLScanCode(KeyCode keyCode);


      std::vector<SDL_Event> frameEvents;
      const SDL2WindowSystem &windowSystem;
};


}

