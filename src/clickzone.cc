#include "clickzone.h"
#include "main.h"
#include <math.h>

std::vector<Clickzone> clickzones;
int selected_zone = 0;
static int move_cooldown = 0;

void clickzone_update(){
    move_cooldown--;
}

void clickzone_move(int x, int y){
    // the "real" position, where our focus is now.
    int rx = clickzones[selected_zone].x, ry = clickzones[selected_zone].y;

    // Velocity
    int magnitude_sqr = x*x+y*y;
    double magnitude = sqrt(magnitude_sqr);

    // Direction of motion
    double sx = double(x)/magnitude;
    double sy = double(y)/magnitude;

    if (magnitude>=3 && move_cooldown<0){
        double best_score = 0;
        int best_idx = -1;
        for (int idx = 0; idx<(int)clickzones.size(); idx++) {
            if (idx==selected_zone) continue;

            // cx is the vector from us towards the 
            int cx = clickzones[idx].x-rx;
            int cy = clickzones[idx].y-ry;
            double cm = sqrt(cx*cx+cy*cy);
            
            // Normalized 
            double scx = double(cx)/cm;
            double scy = double(cy)/cm;

            double dotproduct = scx*sx + scy*sy;

            // No angles > 45 degrees.
            if (dotproduct<0.7) continue;

            // squared dot product just to weight it more..
            // playtesting the feel of this on PS2 will be important.
            double score = dotproduct*dotproduct/cm*clickzones[idx].scale;
            if (score>best_score) {
                best_score = score;
                best_idx = idx;
            }
        }
        if (best_idx!=-1) {
            printf("Moved to %d with score of %.10lf\n", best_idx, best_score);
            move_cooldown=15;
            selected_zone = best_idx;
        }
    }
}
void clickzone_draw(){
    //TODO: Once decals are a thing, this square rendering will be unnecessary.
    for (int idx = 0; idx<(int)clickzones.size(); idx++) {
        int rx = clickzones[idx].x, ry = clickzones[idx].y;
        {
            int const SIZE = 6;
            for(int x = rx-SIZE; x<rx+SIZE; x++) for(int y = ry-SIZE; y<ry+SIZE; y++)
                ((uint32_t *) screen->pixels)[y*screen->w+x] = 0xFFFFFFFF;
        }
        {
            int const SIZE = 2;
            for(int x = rx-SIZE; x<rx+SIZE; x++) for(int y = ry-SIZE; y<ry+SIZE; y++)
                ((uint32_t *) screen->pixels)[y*screen->w+x] = 0xFF00AA00;
        }
    }
    {
        int rx = clickzones[selected_zone].x, ry = clickzones[selected_zone].y;
        int const SIZE = 5;
        for(int x = rx-SIZE; x<rx+SIZE; x++) for(int y = ry-SIZE; y<ry+SIZE; y++)
            ((uint32_t *) screen->pixels)[y*screen->w+x] = ((move_cooldown<0)?0xFFFF0000:0xFF00AAFF);
    }
}