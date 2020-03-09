#include <SDL/SDL.h>

void setBackground(char const * background_path);
char const *getBackground();
void background_draw();

SDL_Surface *load_image(char const *const filename);

extern SDL_Surface *screen;
