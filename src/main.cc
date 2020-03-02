#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
// Woo, I love C strings!
#include <string.h>
#include <assert.h>

#include "script.h"
#include "clickzone.h"
#include "main.h"

SDL_Surface *load_image( char const * filename ) {
    SDL_Surface* raw, *optimized = NULL;

    if ((raw = IMG_Load(filename))) {
        optimized = SDL_DisplayFormat(raw);
        SDL_FreeSurface(raw);
    }

    return optimized;
}

// Background
static char *background_path = NULL;
static SDL_Surface *background_image = NULL;

SDL_Surface *screen = NULL;

void setBackground(char const * path){
    size_t len = strlen(path);
    background_path = (char *) realloc(background_path, len);
    strcpy(background_path, path);
    assert(background_image = load_image(background_path));
}

char const *getBackground(){
    return background_path;
}

int main(int argc, char **argv) {
    
    if (SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
        return EXIT_FAILURE;
    }

    if (!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE))) {
        return EXIT_FAILURE;
    }

    SDL_WM_SetCaption("BRETON", NULL);

    execScript("Kitchen_N.txt");

    SDL_Rect offset;
    offset.x = 0;
    offset.y = 0;
    
    bool stop = false;
    while (!stop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION:{
                    int x = event.motion.xrel, y = event.motion.yrel;
                    clickzone_move(x, y);
                    break;
                }
                case SDL_QUIT:
                    stop = true;
                    break;
                default:
                    break;
            }
        }

        clickzone_update();

        // Draw background
        if (background_image) SDL_BlitSurface(background_image, NULL, screen, &offset);

        clickzone_draw();

        if (SDL_Flip(screen) == -1) {
            break;
        }
        SDL_Delay(16);
    }

    saveScript();

    if (background_image) SDL_FreeSurface(background_image);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
