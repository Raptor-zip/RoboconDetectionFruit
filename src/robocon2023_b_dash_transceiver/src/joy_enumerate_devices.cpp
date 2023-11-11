#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <cstdio>
#include <stdexcept>

int main(int argc, char ** argv)
{
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_HAPTIC) < 0) {
    fprintf(stderr, "SDL could not be initialized: %s\n", SDL_GetError());
    return 1;
  }
  fprintf(stdout, "Joystick Device ID : Joystick Device Name\n");
  fprintf(stdout, "-----------------------------------------\n");
  for (int i = 0; i < SDL_NumJoysticks(); ++i) {
    fprintf(stdout, "%18d : %s\n", i, SDL_JoystickNameForIndex(i));
  }
  SDL_Quit();

  return 0;
}
