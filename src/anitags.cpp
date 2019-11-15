#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <filesystem>
#include <string.h>

#include "../inc/ExifTool.h"
#include "../inc/command_line.h"
#include "../inc/error_codes.h"
#include "../inc/psqlConnection.h"
#include "../inc/config.h"

using namespace std;
//namespace fs = std::filesystem;

void remove_tags(config *conf, ExifTool *et);
void add_tags(config *conf, ExifTool *et);
void write_tags(config *conf, ExifTool *et, char* argv0);
void search_by_tags(config *conf, ExifTool *et, char* argv0);
vector<string> get_tags(string filepath, ExifTool *et, char* argv0);
vector<string> split_string(string list, string delimiter);
void print_tags(config *conf, ExifTool *et, char* argv0);

int main(int argc, char* argv[]) {

    config *conf = (config *) calloc(1, sizeof(config));

    processArgs(argc, argv, conf);

    ExifTool *et = new ExifTool();

    db_connect("postgres", "127.0.0.1", 5432);

    if (conf->tags_to_remove.size() > 0) {
        db_remove_tags(conf);
        if (conf->modifyExif)
            remove_tags(conf, et);
    }

    if (conf->tags_to_add.size() > 0) {
        db_add_tags(conf);
        if (conf->modifyExif)
            add_tags(conf, et);
    }

    if (conf->modifyTags) {
        if (conf->modifyExif)
            write_tags(conf, et, argv[0]);
    }
    else if (conf->searchByTags) {
        /*vector<string> filenames = db_search_for_tags(conf);
        for (string filename : filenames) {
            printf("%s\n", filename.c_str());
        }*/
        search_by_tags(conf, et, argv[0]);
    }
    else if (conf->listTags) {
        db_list_tags(conf);
    }
    else {
        print_tags(conf, et, argv[0]);
    }

    delete et;
    free(conf);

    return SUCCESS;
}

void remove_tags(config *conf, ExifTool *et) {
    for(int i=0; i<conf->tags_to_remove.size(); i++) {
        et->SetNewValue("XMP:Anitags-", conf->tags_to_remove[i].c_str());
    }
}

void add_tags(config *conf, ExifTool *et) {
    for(int i=0; i<conf->tags_to_add.size(); i++) {
        et->SetNewValue("XMP:Anitags-", conf->tags_to_add[i].c_str());
        et->SetNewValue("XMP:Anitags+", conf->tags_to_add[i].c_str());
    }
}

void search_by_tags(config *conf, ExifTool *et, char* argv0) {
    vector<string> filenames = db_search_for_tags(conf);
    sort(filenames.begin(), filenames.end());
    vector<string> suffixes = {".png", ".PNG", ".jpg", ".JPG", ".JPEG", ".jpeg"};
    for (string dirname : conf->dirnames) {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(dirname)) {
            string file = entry.path();
            int suffix_pos = file.find_last_of(".");
            if (suffix_pos==string::npos)
                continue;
            if (find(suffixes.begin(), suffixes.end(), file.substr(suffix_pos))!=suffixes.end()) {
                conf->filenames.push_back(entry.path());
            }
        }
    }
    for (string file : conf->filenames) {
        //printf("CHEKCING FILE: %s", file.c_str());
        if (binary_search(filenames.begin(), filenames.end(), file.substr(file.find_last_of("/")+1))) {
            printf("%s\n", file.c_str());
        }
    }
    /*for (string dirname : conf->dirnames) {
        for (const auto &entry : std::filesystem::directory_iterator(dirname)) {
            conf->filenames.push_back(entry.path());
        }
        sort(conf->filenames.begin(), conf->filenames.end());
        for (string filename : conf->filenames) {
            vector<string> tags = get_tags(filename, et, argv0);
            sort(tags.begin(), tags.end());
            if (includes(tags.begin(), tags.end(), conf->tags_to_search.begin(), conf->tags_to_search.end()))
                printf("%s\n", filename.c_str());
        }
        //print_tags(conf, et, argv0);
        conf->filenames.clear();
    }*/
}

void write_tags(config *conf, ExifTool *et, char* argv0) {
    for (string filename : conf->filenames) {
        et->WriteInfo(filename.c_str(), "-overwrite_original_in_place");
        int result = et->Complete();
        if (result <= 0) {
            printf("Error: could not write tags\n");
            printf("Try '%s -h' for more information\n", argv0);
            exit(ERROR_EXECUTING_EXIFTOOL);
        }
    }
}

void print_tags(config *conf, ExifTool *et, char* argv0) {
    vector<pair<string, vector<string>>> tag_list = db_get_tags(conf);
    for (pair<string, vector<string>> p : tag_list) {
        if (tag_list.size()>1)
            printf("%s:\n", p.first.c_str());
        for (string tag : p.second) {
            if (tag==p.second[0])
                printf("%s", tag.c_str());
            else
                printf(", %s", tag.c_str());
        }
        if (p.second.size()>0)
            printf("\n");
    }
    /*for (string filename : conf->filenames) {
        vector<string> tags = get_tags(filename, et, argv0);
        if (tags.size()>0) {
            printf("%s", tags[0].c_str());
            for (int i=1; i<tags.size(); i++) {
                printf(", %s", tags[i].c_str());
            }
            printf("\n");
        }
    }*/
}

vector<string> get_tags(string filepath, ExifTool *et, char* argv0) {
    TagInfo *info = et->ImageInfo(filepath.c_str(), "\n-Anitags");
    vector<string> tags;
    if (info) {
        if (TagInfo *i = info->next) {
            tags = split_string(string(i->value), ", ");
        }
        delete info;
    } else if (et->LastComplete() <= 0) {
        printf("Error: could not extract tags\n");
        printf("Try '%s -h' for more information\n", argv0);
        exit(ERROR_EXECUTING_EXIFTOOL);
    }
    return tags;
}

const std::string WHITESPACE = " \n\r\t\f\v";

vector<string> split_string(string slist, string delimiter) {
    vector<string> list;
    auto start = 0U;
    auto end = slist.find(delimiter);
    while (end != string::npos) {
        string s = slist.substr(start, end - start);
        list.push_back(s);
        start = end + delimiter.length();
        end = slist.find(delimiter, start);
    }
    list.push_back(slist.substr(start, end));
    return list;
}

/*void check_file_exists(int argc, char* argv[], string* name) {
    struct stat buffer;
    if (stat(name->c_str(), &buffer)) {
        printf("Error: file not found - %s\n", name->c_str());
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_DOES_NOT_EXIST);
    }
    /*else if (!S_ISREG(buffer.st_mode)) {
        printf("Error: argument is a directory - %s\n", name->c_str());
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_ARGUMENT_IS_DIRECTORY);
    }*/
//}*/

