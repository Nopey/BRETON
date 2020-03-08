#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "main.h"

static SDL_Color const color = { 255, 255, 255 };
static TTF_Font *font = NULL;

void textbox_init(){
    assert(TTF_Init()!=-1);
    assert(font = TTF_OpenFont( "/usr/share/fonts/TTF/Hack-Bold.ttf", 24 ));
}

void textbox(const char *text){
    SDL_Surface *message;
    assert(message = TTF_RenderText_Solid( font, text, color ));


    SDL_Rect offset;
    offset.x = 0;
    offset.y = 0;
    SDL_BlitSurface(message,  NULL, screen, &offset);

    assert(SDL_Flip(screen)!=-1);

    for(;;){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN: if(event.button.button == SDL_BUTTON_LEFT){
                    SDL_FreeSurface(message);
                    return;
                }
                case SDL_QUIT:
                    printf("TODO: Proper exiting from textbox quit\n");
                    exit(1);
                    break;
                default:
                    break;
            }
        }
        SDL_Delay(16);
    }
}