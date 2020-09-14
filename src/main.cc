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

//TODO: Cache the background
void setBackground(char const * path){
    if (background_path && !strcmp(background_path, path)) return;
    if (background_image) SDL_FreeSurface(background_image);
    size_t len = strlen(path);
    background_path = (char *) realloc(background_path, len);
    strcpy(background_path, path);
    assert(background_image = load_image(background_path));
}

void background_draw(){
    // Draw a solid color background
    SDL_Rect r;
    r.x = r.y = 0;
    r.w = screen->w;
    r.h = screen->h;
    SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 0xFF, 0xEE, 0xBB, 0xFF));

    // Try to draw the background_image
    if(!background_image) return;
    SDL_Rect offset;
    offset.x = 0;
    offset.y = 0;
    SDL_BlitSurface(background_image, NULL, screen, &offset);
}

char const *getBackground(){
    return background_path;
}

/* #ifdef _EE
 #define LOG(...) do{ \
    FILE *f = fopen("log.txt", "a"); \
    if(f){ \
        fprintf(f, __VA_ARGS__); \
        fclose(f); \
    } \
} while(0)
#else
*/
#define LOG printf
// #endif

int main(int argc, char **argv) {
#ifdef _EE
    //On the playstation, initialize the IO Processor
    //  to handle meme-ory cards and USB mass storage.
    void ez_iop();
    ez_iop();

    stderr->fd = stdout->fd = fioOpen("mass:/log.txt", O_WRONLY|O_CREAT);
    stderr->type = stdout->type = 64; // STD_IOBUF_TYPE_MASS
#endif
    LOG("Breton main()\n");

    //Initializing Joystick crashes. Why? I dunno
    // if (SDL_Init( SDL_INIT_EVERYTHING ) == -1) {
    if (SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) == -1) {
        LOG("SDL_Init ERR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    LOG("SDL_Init success!\n");
#ifdef _EE
#define SCREEN_FLAGS SDL_HWSURFACE|SDL_FULLSCREEN
#else
#define SCREEN_FLAGS SDL_HWSURFACE
#endif
    if (!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SCREEN_FLAGS))) {
    // if (!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE))) {
        LOG("SDL_SETVIDEOMODE ERR: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    textbox_init();
    script_init();

    SDL_WM_SetCaption("BRETON", NULL);

    execScript("Kitchen_N.txt");

    //Kinda hackish joystick stuff:
    SDL_Joystick *joy = NULL;
    // Check for joystick
    if(SDL_NumJoysticks()>0){
        // Open joystick
        joy=SDL_JoystickOpen(0);
        
        if(joy)
        {
            LOG("Opened Joystick 0\n");
            LOG("Name: %s\n", SDL_JoystickName(0));
            LOG("Number of Axes: %d\n", SDL_JoystickNumAxes(joy));
            LOG("Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));
            LOG("Number of Balls: %d\n", SDL_JoystickNumBalls(joy));
        }
        else
            LOG("Couldn't open Joystick 0\n");
    }else{
        LOG("No joysticks\n");
    }

    bool stop = false;
    LOG("Successfully entering main loop\n");
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
                case SDL_JOYBUTTONDOWN:
                    if (event.jbutton.which==0 && event.jbutton.button == 1) clickzone_act();
                    break;
                case SDL_QUIT:
                    stop = true;
                    break;
                default:
                    break;
            }
        }

        // Draw background
        background_draw();

        //More hackish joystick stuff
        if(joy){
            int x = SDL_JoystickGetAxis(joy, 0);
            int y = SDL_JoystickGetAxis(joy, 1);
            clickzone_move(x, y);
            /*
            x += 32768;
            y += 32768;
            if(first){
                for(int axis = 0; axis<SDL_JoystickNumAxes(joy); axis++){
                    LOG("Axis %d: %d\n", axis, SDL_JoystickGetAxis(joy, axis));
                }
                first=false;
            }
            SDL_Rect r;
            r.x = 0;
            r.y = 0;
            r.w = screen->w/2;
            r.h = screen->h/2;
            SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 0x00, 0x00, 0x00, 0xFF));
            r.w = x*screen->w/65535;
            r.h = y*screen->h/65535;
            SDL_FillRect(screen, &r, SDL_MapRGBA(screen->format, 0xDE, 0xA5, 0xA4, 0xFF));
            */
        }

        decal_draw();

        if (SDL_Flip(screen) == -1) {
            break;
        }
        SDL_Delay(16);
    }

    saveScript();

    if (background_image) SDL_FreeSurface(background_image);
            
    // Close if opened
    if(joy)
        SDL_JoystickClose(joy);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
