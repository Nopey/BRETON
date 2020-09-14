#include "flag.h"
#include <set>
#include <string>
#include <stdio.h>

static std::set<std::string> flags;

void flag_set(std::string flag, bool value){
    // printf("flag_set '%s'\n", flag.c_str());
    if(value) flags.insert(flag);
    else flags.erase(flag);
}

void flag_flip(std::string flag){
    flag_set(flag, !flag_get(flag));
}

bool flag_get(std::string flag){
    // printf("flag_get '%s'\n", flag.c_str());
    return flags.find(flag)!=flags.end();
}


//TODO: this should take a file as an arg
void _flag_save_script(){
    for (std::set<std::string>::const_iterator f = flags.begin(); f!=flags.end(); f++)
        printf("F+ %s\n", (*f).c_str());
}
