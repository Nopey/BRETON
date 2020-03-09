#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <iostream>
// Woo, I love C strings!
#include <string.h>
#include <assert.h>

#include "script.h"
#include "clickzone.h"
#include "decal.h"
#include "textbox.h"
#include "main.h"

// Screen attributes
int const SCREEN_WIDTH = 640;
int const SCREEN_HEIGHT = 480;
int const SCREEN_BPP = 32;

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
    if (background_image) SDL_FreeSurface(background_image);
    size_t len = strlen(path);
    background_path = (char *) realloc(background_path, len);
    strcpy(background_path, path);
    assert(background_image = load_image(background_path));
}

void background_draw(){
    if(!background_image) return;
    SDL_Rect offset;
    offset.x = 0;
    offset.y = 0;
    SDL_BlitSurface(background_image, NULL, screen, &offset);
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
    textbox_init();

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
                    // int x = event.motion.xrel, y = event.motion.yrel;
                    // clickzone_move(x, y);
                    clickzone_near(event.motion.x, event.motion.y);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN: if(event.button.button == SDL_BUTTON_LEFT){
                    clickzone_act();
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
        background_draw();

        clickzone_draw();
        decal_draw();

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
