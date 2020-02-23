#include "clickzone.h"
#include <stdio.h>

bool loadScreen(char const *const filename){
    clickzones.clear();
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // if (!*line) continue;
        int x, y;
        double scale;
        if (sscanf(line, "%d %d %lf", &x, &y, &scale)!=3) continue;
        printf("Zone %d %d\n", x, y);
        clickzones.push_back(Clickzone{x, y, scale});
    }

    fclose(file);
    return true;
}
