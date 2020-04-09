//
// Created by yo on 2/22/19.
//

#ifndef ANITAGS_CONFIG_H
#define ANITAGS_CONFIG_H

#include <string>
#include <vector>

using namespace std;

typedef struct config_struct {

    vector<string> tags_to_add;
    vector<string> tags_to_remove;
    vector<string> tags_to_search;
    vector<string> filenames;
    vector<string> dirnames;
    bool needsArg;
    bool needsDir;
    bool modifyTags;
    bool modifyExif;
    bool searchByTags;
    bool listTags;
    bool exportTags;
    bool importTags;

} config;

#endif //ANITAGS_CONFIG_H
