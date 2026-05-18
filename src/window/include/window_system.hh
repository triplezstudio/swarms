#pragma once
#include "../../client_common/include/common.hh"
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



  class WindowSystem {
  public:
  virtual void init() = 0;
  virtual void pollEvents() = 0;
  virtual Window* createWindow(WindowDesc desc) = 0;
  virtual void present() = 0;
  virtual GraphicsSurface createSurface(GraphicsInstance& instance) = 0;


  };

}



