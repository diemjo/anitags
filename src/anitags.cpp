#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vector>
//#include <string>
#include <string.h>

#include "../inc/ExifTool.h"
#include "command_line.hpp"
#include "error_codes.h"
#include "config.hpp"

using namespace std;

void check_file_exists(int argc, char* argv[], string* name);

int main(int argc, char* argv[]) {

    config *conf = (config *) calloc(1, sizeof(config));

    processArgs(argc, argv, conf);

    if (conf->needsFile)
        check_file_exists(argc, argv, &(conf->filename));

    ExifTool *et = new ExifTool();

    //remove tags
    if (conf->tags_to_remove.size()>0) {
        for(int i=0; i<conf->tags_to_remove.size(); i++) {
            et->SetNewValue("XMP:Anitags-", conf->tags_to_remove[i].c_str());
        }
    }

    //add tags
    if (conf->tags_to_add.size()>0) {
        for(int i=0; i<conf->tags_to_add.size(); i++) {
            et->SetNewValue("XMP:Anitags-", conf->tags_to_add[i].c_str());
            et->SetNewValue("XMP:Anitags+", conf->tags_to_add[i].c_str());
        }
    }

    //write to file
    if (conf->modifyTags) {
        et->WriteInfo(conf->filename.c_str());
        int result = et->Complete();
        if (result<=0) {
            printf("Error: could not write tags\n");
            printf("Try '%s -h' for more information\n", argv[0]);
            exit(ERROR_EXECUTING_EXIFTOOL);
        }
    }

    //print tags
    TagInfo *info = et->ImageInfo(conf->filename.c_str(), "\n-Anitags");
    if (info) {
        for (TagInfo *i = info->next; i; i = i->next) {
            //if (strcmp(i->name, "SourceFile") != 0)
            printf("%s=%s\n", i->name, i->value);
        }
        delete info;
    } else if (et->LastComplete() <= 0) {
        printf("Error: could not extract tags\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_EXECUTING_EXIFTOOL);
    }

    delete et;

    return SUCCESS;
}

void check_file_exists(int argc, char* argv[], string* name) {
    struct stat buffer;
    if (stat(name->c_str(), &buffer)) {
        printf("Error: file not found - %s\n", name->c_str());
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_DOES_NOT_EXIST);
    }
    else if (!S_ISREG(buffer.st_mode)) {
        printf("Error: argument is a directory - %s\n", name->c_str());
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_ARGUMENT_IS_DIRECTORY);
    }
}

