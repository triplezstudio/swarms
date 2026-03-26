#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
  // Initialize SDL2
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  // Create a Window
  SDL_Window* window = SDL_CreateWindow(
    "SDL2 Windows 11 Test",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    640, 480,
    SDL_WINDOW_SHOWN
  );

  if (!window) {
    std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  std::cout << "Success! SDL2 is linked and running on Windows." << std::endl;

  // Wait 3 seconds so you can actually see the window
  SDL_Delay(3000);

  // Cleanup
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}