#pragma once
#include <defines.h>
#include <sdl2.hh>
#include <map>
#include <iostream>

namespace tz::input {
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
  MIDDLE
};

class TZ_API SDL2InputSystem
{
public:

  SDL2InputSystem(const SDL2InputSystem&) = delete;
  SDL2InputSystem& operator=(const SDL2InputSystem&) = delete;

  static SDL2InputSystem& getInstance();

  // This must be called once a frame to gather the latest inputs
  void update(std::vector<SDL_Event> incomingEvents);

  // The current screen positions of the mouse, origin is top left (0,0).
  void getMouseCoords(int& x, int& y);

  // Single time, e.g. ui button click;
  bool isMouseButtonClicked(MouseButton mouseButton);

  // Continuous
  bool isMouseButtonDown(MouseButton mouseButton);

  // For single key presses, e.g. menu
  bool isKeyPressed(KeyCode keyCode);

  // For continuous tracking, e.g. WASD movement
  bool isKeyDown(KeyCode keyCode);

  private:
      SDL2InputSystem();
      KeyCode getKeyCodeFromSDLEvent(SDL_Event ev);
      SDL_Scancode toSDLScanCode(KeyCode keyCode);
      std::map<KeyCode, bool> keyDownMap;

      std::vector<SDL_Event> frameInputEvents;

      std::vector<uint8_t> prevFrameKeyboardState;
      std::vector<uint8_t> currentFrameKeyboardState;

      uint32_t prevFrameMouseState = 0;
      uint32_t currentFrameMouseState = 0;

      bool isMouseButtonDown(MouseButton mouseButton, uint32_t state);
};


}

