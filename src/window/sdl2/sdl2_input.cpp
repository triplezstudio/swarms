#include <input.hh>
#include <iostream>

namespace tz::input {
bool SDL2InputSystem::isKeyPressed(KeyCode keyCode)
{
  for (auto &e : windowSystem.getFrameEvents())
  {
    if (fromSDLEvent(e) == keyCode) return true;
  }

  return false;
}

bool SDL2InputSystem::isKeyDown(tz::input::KeyCode keyCode)
{
  const Uint8* state = SDL_GetKeyboardState(NULL);
  return state[toSDLScanCode(keyCode)];

}


SDL2InputSystem::SDL2InputSystem(const tz::SDL2WindowSystem &windowSystem) :windowSystem(windowSystem)
{

}

SDL_Scancode SDL2InputSystem::toSDLScanCode(KeyCode keyCode)
{
  switch (keyCode)
  {
    case KeyCode::A: return SDL_SCANCODE_A;
    case KeyCode::B: return SDL_SCANCODE_B;
    case KeyCode::C: return SDL_SCANCODE_C;
    case KeyCode::D: return SDL_SCANCODE_D;
    case KeyCode::E: return SDL_SCANCODE_E;
    case KeyCode::F: return SDL_SCANCODE_F;
    case KeyCode::G: return SDL_SCANCODE_G;
    case KeyCode::H: return SDL_SCANCODE_H;
    case KeyCode::I: return SDL_SCANCODE_I;
    case KeyCode::J: return SDL_SCANCODE_J;
    case KeyCode::K: return SDL_SCANCODE_K;

  }
}

KeyCode SDL2InputSystem::fromSDLEvent(SDL_Event ev)
{
  // We only care about keyboard events
  if (ev.type != SDL_KEYDOWN && ev.type != SDL_KEYUP)
  {
    // You might want to handle this case, perhaps with a KeyCode::NONE
    return KeyCode::ESC;
  }

  switch (ev.key.keysym.sym)
  {
    // Letters
    case SDLK_a:
      return KeyCode::A;
    case SDLK_b:
      return KeyCode::B;
    case SDLK_c:
      return KeyCode::C;
    case SDLK_d:
      return KeyCode::D;
    case SDLK_e:
      return KeyCode::E;
    case SDLK_f:
      return KeyCode::F;
    case SDLK_g:
      return KeyCode::G;
    case SDLK_h:
      return KeyCode::H;
    case SDLK_i:
      return KeyCode::I;
    case SDLK_j:
      return KeyCode::J;
    case SDLK_k:
      return KeyCode::K;
    case SDLK_l:
      return KeyCode::L;
    case SDLK_m:
      return KeyCode::M;
    case SDLK_n:
      return KeyCode::N;
    case SDLK_o:
      return KeyCode::O;
    case SDLK_p:
      return KeyCode::P;
    case SDLK_q:
      return KeyCode::Q;
    case SDLK_r:
      return KeyCode::R;
    case SDLK_s:
      return KeyCode::S;
    case SDLK_t:
      return KeyCode::T;
    case SDLK_u:
      return KeyCode::U;
    case SDLK_v:
      return KeyCode::V;
    case SDLK_w:
      return KeyCode::W;
    case SDLK_x:
      return KeyCode::X;
    case SDLK_y:
      return KeyCode::Y;
    case SDLK_z:
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
      return KeyCode::ESC;
  }
}
void SDL2InputSystem::getMouseCoords(int &x, int &y)
{
  for (auto& e : frameEvents)
  {
    if (e.type == SDL_MOUSEMOTION)
    {
      x = e.motion.x;
      y = e.motion.y;
      break;
    }
  }
}


bool SDL2InputSystem::isMouseButtonDown(MouseButton mouseButton)
{
  int x,y;
  auto buttonState = SDL_GetMouseState(&x,&y);

  switch (mouseButton)
  {
    case MouseButton::LEFT: return buttonState & SDL_BUTTON(SDL_BUTTON_LEFT);
    case MouseButton::RIGHT: return buttonState & SDL_BUTTON(SDL_BUTTON_RIGHT);
    case MouseButton::MIDDLE: return buttonState & SDL_BUTTON(SDL_BUTTON_MIDDLE);
  }
}
bool SDL2InputSystem::isMouseButtonClicked(MouseButton mouseButton)
{
  auto button = SDL_BUTTON_LEFT;
  switch(mouseButton)
  {
    case MouseButton::RIGHT: button = SDL_BUTTON_RIGHT;
    case MouseButton::MIDDLE: button = SDL_BUTTON_MIDDLE;
  }

  for (auto& e: frameEvents)
  {
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
      std::cout << "mbutton down" << std::endl;
      if (e.button.button == button) return true;
    }
  }
  return false;
}

}