#include <SDL/SDL.h>

// Screen attributes
int const SCREEN_WIDTH = 640;
int const SCREEN_HEIGHT = 480;
int const SCREEN_BPP = 32;

void setBackground(char const * background_path);
char const *getBackground();

SDL_Surface *load_image(char const *const filename);

extern SDL_Surface *screen;
