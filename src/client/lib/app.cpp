
module;
#include <SDL2/SDL.h>
#include <iostream>

module app;

import :sdl2;

app::App::App()
{
  // TODO: read from config which backend to use?
  // Default is SDL2 for now
  impl = new sdl2::SDL2App();

}

void app::App::init()
{

  impl->init();
}

void app::App::run()
{
  impl->run();
}


