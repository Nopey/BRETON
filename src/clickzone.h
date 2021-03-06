#include <vector>

struct Clickzone{
    int x, y;
    double scale;
    char *action;
};

//Left mouse button does this
void clickzone_act();

void clickzone_update();
void clickzone_move(int dx, int dy);
void clickzone_near(int x, int y);
void clickzone_draw();
int clickzone_selected();

// used from scripts
void clickzone_add(int x, int y, double scale, const char *action);
void clickzone_clear();

extern std::vector<Clickzone> clickzones;
