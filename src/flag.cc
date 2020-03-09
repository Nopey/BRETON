#include "flag.h"
#include <set>
#include <string>

static std::set<std::string> flags;

void flag_set(std::string flag, bool value){
    if(value) flags.insert(flag);
    else flags.erase(flag);
}

void flag_flip(std::string flag){
    flag_set(flag, !flag_get(flag));
}

bool flag_get(std::string flag){
    return flags.find(flag)!=flags.end();
}


//TODO: this should take a file as an arg
void _flag_save_script(){
    for (auto &f : flags)
        printf("F %s\n", f.c_str());
}
