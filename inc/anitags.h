#ifndef ANITAGS_H
#define ANITAGS_H

#include "../inc/ExifTool.h"
#include "../inc/config.h"

void exif_remove_tags(config *conf, ExifTool *et);
void exif_add_tags(config *conf, ExifTool *et);
void exif_clear_new_tags(ExifTool *et);
void exif_write_tags(config *conf, ExifTool *et, char* argv0);
void search_by_tags(config *conf, ExifTool *et, char* argv0);
vector<string> exif_get_tags(string filepath, ExifTool *et, char* argv0);
vector<string> split_string(string list, string delimiter);
void print_tags(config *conf, ExifTool *et, char* argv0);
void import_tags(config *conf, ExifTool *et, char* argv0);
void export_tags(config *conf, ExifTool *et, char* argv0);

#endif //ANITAGS_H
