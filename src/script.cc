#include "clickzone.h"
#include "main.h"
#include <stdio.h>

char *trim(char *stmt){
    while (*stmt==' ' || *stmt=='\t') stmt++;
    return stmt;
}

bool execScript(char const *const filename){
    clickzones.clear();
    FILE* file = fopen(filename, "r");
    if (!file) return false;
    char line[256];

    // used + and -, set by ? and !
    bool flag = false;

    while (fgets(line, sizeof(line), file)) {
        // Cut whitespace
        char *stmt = trim(line);

        // If there is a comment, cut it off
        {
            char *comment = stmt;
            while(*comment && *comment!='#' && *comment!='\n' && (*comment!='/' || *(comment+1)!='/')) comment++;
            *comment = '\0';
        }

        if (*stmt=='+' || *stmt=='-'){
            if ((*stmt=='+') ^ flag) continue;
            else stmt = trim(stmt+1);
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
            case 'B': case 'b': // Set background
                setBackground(trim(stmt+1));
                break;
            case '?': // Check for flag
                flag = true; //TODO: FLAGS
                break;
            case '!': // Not
                flag = !flag;
                break;
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

void saveScript(){
    //TODO: Write to a file, write to the PS2 Memory card.
    printf(
        "# BRETON SAVE\n\n# Background\nB %s\n\n",
        getBackground()
    );
    //TODO: Save clickzones
}
