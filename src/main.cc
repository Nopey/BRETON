#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
#include "load_screen.h"
#include "clickzone.h"
#include "main.h"

// Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

SDL_Surface *load_image( char const *const filename ) {
    SDL_Surface* raw, *optimized = NULL;

    if ((raw = IMG_Load(filename))) {
        optimized = SDL_DisplayFormat(raw);
        SDL_FreeSurface(raw);
    }

    return optimized;
}

// Background
static SDL_Surface *image = NULL;

SDL_Surface *screen = NULL;

int main(int argc, char **argv) {
    
    if (SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
        return EXIT_FAILURE;
    }

    if (!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE))) {
        return EXIT_FAILURE;
    }

    SDL_WM_SetCaption( "BRETON", NULL );

    if (!(image = load_image( "Kitchen_N.png" ))) {
        SDL_Quit();
        return EXIT_FAILURE;
    }

    loadScreen("Kitchen_N.txt");

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
        SDL_BlitSurface( image, NULL, screen, &offset );

        clickzone_draw();

        if (SDL_Flip( screen ) == -1) {
            break;
        }
        SDL_Delay(16);
    }

    SDL_FreeSurface( image );
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
