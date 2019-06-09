//
// Created by yo on 2/22/19.
//

#ifndef ANITAGS_CONFIG_HPP
#define ANITAGS_CONFIG_HPP

#include <string>
#include <vector>

using namespace std;

typedef struct config_struct {

    vector<string> tags_to_add;
    vector<string> tags_to_remove;
    vector<string> tags_to_search;
    string filename;
    bool needsFile;
    bool modifyTags;
    bool searchByTags;

} config;

#endif //ANITAGS_CONFIG_HPP
