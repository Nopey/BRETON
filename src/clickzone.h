#include <vector>

struct Clickzone{
    int x, y;
    double scale;
};

void clickzone_update();
void clickzone_move(int x, int y);
void clickzone_draw();

extern int selected_zone;
extern std::vector<Clickzone> clickzones;
