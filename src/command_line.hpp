//
// Created by yo on 2/22/19.
//

#ifndef ANITAGS_COMMAND_LINE_HPP
#define ANITAGS_COMMAND_LINE_HPP

#include <stdio.h>
#include <string>
#include <vector>

#include "error_codes.h"
#include "config.hpp"

using namespace std;

void printUsage(int argc, char* argv[]);
void processArgs(int argc, char* argv[], config* conf);

void processArgs(int argc, char* argv[], config* conf) {
    int c;
    conf->needsFile=true;
    while ((c = getopt(argc, argv, "t:r:s:hl")) != -1) {
        switch (c) {
            case 't':
                conf->tags_to_add.push_back(optarg);
                conf->modifyTags=true;
                //conf->needsFile=true;
                break;
            case 'r':
                conf->tags_to_remove.push_back(optarg);
                conf->modifyTags=true;
                //conf->needsFile=true;
                break;
            case 's':
                conf->tags_to_search.push_back(optarg);
                conf->needsFile=false;
                break;
            case 'l':
                //conf->needsFile=true;
                break;
            case 'h':
                conf->needsFile=false;
                printUsage(argc, argv);
                exit(SUCCESS);
            default:
                conf->needsFile=false;
                printUsage(argc, argv);
                exit(ERROR_INVALID_ARGUMENT);
                break;
        }
    }
    if (optind < argc) {
        char* path = realpath(argv[optind], NULL);
        if (!path) {
            printf("Error: file not found - %s\n", argv[optind]);
            printf("Try '%s -h' for more information\n", argv[0]);
            exit(ERROR_FILE_DOES_NOT_EXIST);
        }
        string str(path);
        conf->filename = str;
        //printf("%s\n", path);
        free(path);
    }
    else if (conf->needsFile) {
        printf("Error: file required\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_REQUIRED);
    }
    if ((conf->tags_to_search.size()) > 0 && conf->modifyTags) {
        printf("Error: cannot use option '-s' combined with option '-t' or '-r'");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_OPTION_S_WITH_T_R);
    }
}

void printUsage(int argc, char* argv[]) {
    printf("Usage: %s [OPTIONS] [FILE]\n", argv[0]);
    printf("\n");
    printf("Options:\n");
    printf("    -t TAGS       Add tag to image (requires file argument)\n");
    printf("    -r TAGS       Remove tag from image (requires file argument)\n");
    printf("    -l            List tags of file (requires file argument). default on.\n");
    printf("    -s TAGS       Filter directory for images matching the tags\n");
}

#endif //ANITAGS_COMMAND_LINE_HPP
