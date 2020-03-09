#include "clickzone.h"
#include "decal.h"
#include "main.h"
#include "textbox.h"
#include <stdio.h>
#include "flag.h"

char *trim(char *stmt){
    while (*stmt==' ' || *stmt=='\t') stmt++;
    return stmt;
}

bool execScript(char const *const filename){
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

        //TODO: Scatter the script loading elements to their files, like the saving?
        switch(*stmt){
            // Zone
            case 'Z': case 'z': {
                int x, y;
                double scale;
                char action_filename[256];
                action_filename[0] = '\0';
                if (sscanf(stmt+1, "%d %d %lf %s", &x, &y, &scale, action_filename)<3) return false;
                printf("Zone %d %d\n", x, y);
                clickzone_add(x, y, scale, *action_filename ? action_filename : NULL);
                break;
            }
            // Decals
            case 'D': case 'd': {
                int x, y;
                char filename[256];
                if (sscanf(stmt+1, "%d %d %s", &x, &y, filename)!=3) return false;
                printf("Decal %d %d %s\n", x, y, filename);
                decal_add(x, y, filename);
                break;
            }
            // Set background
            case 'B': case 'b':
                setBackground(trim(stmt+1));
                break;
            // Reset/Clear Clickzones and decals
            case 'R': case 'r': case 'C': case 'c':
                // TODO: Finalize letter.
                clickzone_clear();
                decal_clear();
                break;
            case 'F': case 'f':{
                stmt = trim(++stmt);
                switch (*stmt){
                    case '+':
                        flag_set(std::string(trim(++stmt)), true);
                        break;
                    case '-':
                        flag_set(std::string(trim(++stmt)), false);
                        break;
                    case '!':
                        flag_flip(std::string(trim(++stmt)));
                        break;
                    default:
                        printf("UNKNOWN FLAG COMMAND 0x%02x\n", *stmt);
                        break;
                }
            }
            case '?': // Check for flag
                // Check for flag
                flag = flag_get(std::string(trim(stmt+1)));
                break;
            case '!': // Not
                flag = !flag;
                break;
            // Open a text message
            case 'T': case 't':
                printf("Say '%s'\n", trim(stmt+1));
                textbox(trim(stmt+1));
                break;
            // Execute a file
            case 'X': case 'x':
                fclose(file);
                return execScript(trim(stmt+1));
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

//TODO: these should take a file as an arg
void _clickzone_save_script();
void _decal_save_script();
void _flag_save_script();

void saveScript(){
    //TODO: Write to a file, write to the PS2 Memory card.
    printf("# BRETON SAVE\n\n");

    char const *bg = getBackground();
    if (bg) printf("# Background\nB %s\n\n", bg);

    printf("# Clickzones:\n");
    _clickzone_save_script();
    
    printf("\n# Decals:\n");
    _decal_save_script();

    printf("\n# Flags:\n");
    _flag_save_script();
}
