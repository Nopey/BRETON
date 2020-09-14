#include "main.h"
#include "clickzone.h"
#include <vector>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <string.h>
#include <assert.h>

struct Decal{
    int x, y;
    char *filename;
    SDL_Surface *image, *glow;
};

static std::vector<Decal> decals;

void decal_add(int x, int y, char const *filename){
    SDL_Surface *image, *glow;
    
    image = IMG_Load(filename);
    assert(image);

    SDL_PixelFormat *f = image->format;

    glow = SDL_CreateRGBSurface
        (SDL_SWSURFACE, image->w, image->h, f->BitsPerPixel, 
        f->Rmask, f->Gmask, f->Bmask, f->Amask);
    {
        SDL_Rect r;
        r.x = r.y = 0;
        r.w = image->w;
        r.h = image->h;
        SDL_FillRect(glow, &r, 0);
    }
    Uint32 yellow = f->Rmask | f->Gmask | f->Amask;
    int const OFFSETS[8][2] = {
        { 1, 0},
        { 1, 1},
        { 0, 1},
        {-1, 1},
        {-1, 0},
        {-1,-1},
        { 0,-1},
        { 1,-1}
    };
    for(int x=0; x<image->w; x++) for(int y=0; y<image->h; y++){
        bool solid = false;
        for(int o=0; o<9; o++){
            int nx = x + OFFSETS[o][0];
            int ny = y + OFFSETS[o][1];
            if(nx<0 || ny<0 || nx>=image->w || ny>=image->h ) continue;
            if(((Uint32*)image->pixels)[nx+ny*image->pitch/4] & f->Amask){
                solid=true;
                break;
            }
        }
        if (solid) ((Uint32*)glow->pixels)[x+y*glow->pitch/4] = yellow;
    }

    Decal decal = {
        x, y, strdup(filename),
        image, glow
    };
    decals.push_back(decal);
}

void decal_clear(){
    decals.clear();
}

void decal_draw(bool draw_highlight){
    int did=0;
    for(unsigned int x=0; x<decals.size(); x++){
        Decal &d = decals[x];
        SDL_Rect offset;
        offset.x = d.x;
        offset.y = d.y;
        if (draw_highlight && clickzone_selected()==did++) SDL_BlitSurface(d.glow,  NULL, screen, &offset);
        SDL_BlitSurface(d.image, NULL, screen, &offset);
    }
}

void decal_move(int x, int y, int id){
    if (id<0 || ((unsigned int)id)>=decals.size()) return;
    decals[id].x = x;
    decals[id].y = y;
}

//TODO: this should take a file as an arg
void _decal_save_script(){
    for(unsigned int x=0; x<decals.size(); x++){
        Decal &d = decals[x];
        printf("D %d %d %s\n", d.x, d.y, d.filename);
    }
}
