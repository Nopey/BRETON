#include "clickzone.h"
#include <stdio.h>

bool loadScreen(char const *const filename){
    clickzones.clear();
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // Cut whitespace
        char *stmt = line;
        while (*stmt==' ' || *stmt=='\t') stmt++;

        // If there is a comment, cut it off
        {
            char *comment = stmt;
            while(*comment && *comment!='#' && (*comment!='/' || *(comment+1)!='/')) comment++;
            *comment = '\0';
        }

        switch(*stmt){
            case 'Z': case 'z': { // Zone
                int x, y;
                double scale;
                if (sscanf(stmt, "z %d %d %lf", &x, &y, &scale)!=3) continue;
                printf("Zone %d %d\n", x, y);
                clickzones.push_back(Clickzone{x, y, scale});
                break;
            }
            case '\0': // empty lines
                break;
            default:
                printf("Unknown load screen statement '%c'\n", *stmt);
                break;
        }
    }

    fclose(file);
    return true;
}
