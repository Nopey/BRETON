#include <SDL/SDL_ttf.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "decal.h"
#include "main.h"

static SDL_Color const color = { 255, 255, 255 };
static TTF_Font *font = NULL;

void textbox_init(){
    assert(TTF_Init()!=-1);
    //TODO: Free the TTF font
    assert(font = TTF_OpenFont( "Hack-Bold.ttf", 24 ));
}

void textbox(const char *text){
    SDL_Surface *message;
    assert(message = TTF_RenderText_Solid( font, text, color ));

    background_draw();
    decal_draw();

    SDL_Rect offset;
    offset.x = screen->w/2 - message->w/2;
    offset.y = screen->h - message->h - 10;
    SDL_BlitSurface(message,  NULL, screen, &offset);

    assert(SDL_Flip(screen)!=-1);

    for(bool done=false; !done;){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    if(event.button.button == SDL_BUTTON_LEFT) done=true;
                    break;
                    case SDL_JOYBUTTONDOWN:
                    if (event.jbutton.button == 2){
                        char buf[100]; snprintf(buf, 100, "Controller #%d", event.jbutton.which); textbox(buf);
                    }
                    if (event.jbutton.which==0 && event.jbutton.button == 1) done=true;
                    break;
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
    SDL_FreeSurface(message);
}
