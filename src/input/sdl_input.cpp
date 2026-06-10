#include <input.hh>
#include <iostream>

namespace tz::input {

// Key "pressed" means: was the key down in the previous frame and is now up?
bool SDLInputSystem ::isKeyPressed(KeyCode keyCode)
{
  for (auto &e : frameInputEvents)
  {
    if (e.type == SDL_EVENT_KEY_DOWN && getKeyCodeFromSDLEvent(e) == keyCode)
    {
      return true;
    }
  }

  return false;
}

bool SDLInputSystem ::isKeyDown(KeyCode keyCode)
{
  return currentFrameKeyboardState[toSDLScanCode(keyCode)];
}

SDL_Scancode SDLInputSystem ::toSDLScanCode(KeyCode keyCode)
{
  switch (keyCode)
  {
    case KeyCode::A:
      return SDL_SCANCODE_A;
    case KeyCode::B:
      return SDL_SCANCODE_B;
    case KeyCode::C:
      return SDL_SCANCODE_C;
    case KeyCode::D:
      return SDL_SCANCODE_D;
    case KeyCode::E:
      return SDL_SCANCODE_E;
    case KeyCode::F:
      return SDL_SCANCODE_F;
    case KeyCode::G:
      return SDL_SCANCODE_G;
    case KeyCode::H:
      return SDL_SCANCODE_H;
    case KeyCode::I:
      return SDL_SCANCODE_I;
    case KeyCode::J:
      return SDL_SCANCODE_J;
    case KeyCode::K:
      return SDL_SCANCODE_K;
  }
}

KeyCode SDLInputSystem ::getKeyCodeFromSDLEvent(SDL_Event ev)
{
  // We only care about keyboard events
  if (ev.type != SDL_EVENT_KEY_DOWN && ev.type != SDL_EVENT_KEY_UP)
  {
    // You might want to handle this case, perhaps with a KeyCode::NONE
    return KeyCode::NONE;
  }

  switch (ev.key.key)
  {
    // Letters
    case SDLK_A:
      return KeyCode::A;
    case SDLK_B:
      return KeyCode::B;
    case SDLK_C:
      return KeyCode::C;
    case SDLK_D:
      return KeyCode::D;
    case SDLK_E:
      return KeyCode::E;
    case SDLK_F:
      return KeyCode::F;
    case SDLK_G:
      return KeyCode::G;
    case SDLK_H:
      return KeyCode::H;
    case SDLK_I:
      return KeyCode::I;
    case SDLK_J:
      return KeyCode::J;
    case SDLK_K:
      return KeyCode::K;
    case SDLK_L:
      return KeyCode::L;
    case SDLK_M:
      return KeyCode::M;
    case SDLK_N:
      return KeyCode::N;
    case SDLK_O:
      return KeyCode::O;
    case SDLK_P:
      return KeyCode::P;
    case SDLK_Q:
      return KeyCode::Q;
    case SDLK_R:
      return KeyCode::R;
    case SDLK_S:
      return KeyCode::S;
    case SDLK_T:
      return KeyCode::T;
    case SDLK_U:
      return KeyCode::U;
    case SDLK_V:
      return KeyCode::V;
    case SDLK_W:
      return KeyCode::W;
    case SDLK_X:
      return KeyCode::X;
    case SDLK_Y:
      return KeyCode::Y;
    case SDLK_Z:
      return KeyCode::Z;

    // Numbers
    case SDLK_0:
      return KeyCode::NUM_0;
    case SDLK_1:
      return KeyCode::NUM_1;
    case SDLK_2:
      return KeyCode::NUM_2;
    case SDLK_3:
      return KeyCode::NUM_3;
    case SDLK_4:
      return KeyCode::NUM_4;
    case SDLK_5:
      return KeyCode::NUM_5;
    case SDLK_6:
      return KeyCode::NUM_6;
    case SDLK_7:
      return KeyCode::NUM_7;
    case SDLK_8:
      return KeyCode::NUM_8;
    case SDLK_9:
      return KeyCode::NUM_9;

    // Special Keys
    case SDLK_RETURN:
      return KeyCode::ENTER;
    case SDLK_ESCAPE:
      return KeyCode::ESC;
    case SDLK_UP:
      return KeyCode::UP;
    case SDLK_DOWN:
      return KeyCode::DOWN;
    case SDLK_LEFT:
      return KeyCode::LEFT;
    case SDLK_RIGHT:
      return KeyCode::RIGHT;

    // Modifiers
    case SDLK_LCTRL:
      return KeyCode::CTRL_LEFT;
    case SDLK_RCTRL:
      return KeyCode::CTRL_RIGHT;
    case SDLK_LSHIFT:
      return KeyCode::SHIFT_LEFT;
    case SDLK_RSHIFT:
      return KeyCode::SHIFT_RIGHT;

    // Function Keys
    case SDLK_F1:
      return KeyCode::F1;
    case SDLK_F2:
      return KeyCode::F2;
    case SDLK_F3:
      return KeyCode::F3;
    case SDLK_F4:
      return KeyCode::F4;
    case SDLK_F5:
      return KeyCode::F5;
    case SDLK_F6:
      return KeyCode::F6;
    case SDLK_F7:
      return KeyCode::F7;
    case SDLK_F8:
      return KeyCode::F8;
    case SDLK_F9:
      return KeyCode::F9;
    case SDLK_F10:
      return KeyCode::F10;
    case SDLK_F11:
      return KeyCode::F11;
    case SDLK_F12:
      return KeyCode::F12;

    default:
      // Handle unmapped keys here
      return KeyCode::NONE;
  }
}

void SDLInputSystem ::getMouseCoords(float &x, float &y)
{
  SDL_GetMouseState(&x, &y);
}

bool SDLInputSystem ::isMouseButtonDown(tz::input::MouseButton mouseButton, uint32_t state)
{
  switch (mouseButton)
  {
    case MouseButton::LEFT:
      return state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
    case MouseButton::RIGHT:
      return state & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
    case MouseButton::MIDDLE:
      return state & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE);
  }
}

bool SDLInputSystem ::isMouseButtonDown(MouseButton mouseButton)
{
  return isMouseButtonDown(mouseButton, currentFrameMouseState);
}
bool SDLInputSystem ::isMouseButtonClicked(MouseButton mouseButton)
{
  return isMouseButtonDown(mouseButton, prevFrameMouseState)
         && !isMouseButtonDown(mouseButton, currentFrameMouseState);
}

void SDLInputSystem ::update(std::vector<SDL_Event> incomingEvents)
{
  frameInputEvents = incomingEvents;
}
SDLInputSystem ::SDLInputSystem()
{
  int numKeys = 0;
  SDL_GetKeyboardState(&numKeys);
  prevFrameKeyboardState.resize(numKeys);
  currentFrameKeyboardState.resize(numKeys);
}

SDLInputSystem &SDLInputSystem ::getInstance()
{
  static SDLInputSystem instance;
  return instance;
}
} // namespace tz::input
