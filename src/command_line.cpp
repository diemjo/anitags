#include "../inc/error_codes.h"
#include "../inc/config.h"
#include "../inc/command_line.h"
#include <cstring>

using namespace std;

void processArgs(int argc, char* argv[], config* conf) {
    int c;
    struct option options[] = {
            {"add-tag", required_argument, 0, 't'},
            {"remove-tag", required_argument, 0, 'r'},
            {"filter-tag", required_argument, 0, 'f'},
            {"exif", no_argument, 0, 'e'},
            {"help", no_argument, 0, 'h'},
            {"list-tags", no_argument, 0, 'l'},
            {"import-tags", no_argument, 0, 0},
            {"export-tags", no_argument, 0, 0}
    };
    int option_index = 0;
    conf->needsArg=true;
    if (argc==1) {
	printUsage(argc, argv);
	exit(SUCCESS);
    }
    while ((c = getopt_long(argc, argv, "t:r:f:ehl", options, &option_index)) != -1) {
        switch (c) {
            case 0:
                if (strncmp(options[option_index].name, "export-tags", 12)==0) {
                    conf->exportTags=true;
                }
                else if (strncmp(options[option_index].name, "import-tags", 12)==0) {
                    conf->importTags=true;
                }
                break;
            case 't':
                conf->tags_to_add.push_back(optarg);
                conf->modifyTags=true;
                break;
            case 'r':
                conf->tags_to_remove.push_back(optarg);
                conf->modifyTags=true;
                break;
            case 'f':
                conf->tags_to_search.push_back(optarg);
                conf->searchByTags=true;
                conf->needsDir=true;
                conf->needsArg=false;
                break;
            case 'e':
                conf->modifyExif=true;
                break;
            case 'l':
                conf->needsArg=false;
                conf->listTags=true;
                break;
            case 'h':
                printUsage(argc, argv);
                exit(SUCCESS);
            default:
                printUsage(argc, argv);
                exit(ERROR_INVALID_ARGUMENT);
                break;
        }
    }

    int actions = (conf->exportTags ? 1 : 0) + (conf->importTags ? 1 : 0) + (conf->modifyTags ? 1 : 0) +  (conf->searchByTags ? 1 : 0) + (conf->listTags ? 1 : 0);
    if (actions>1) {
        printf("Error: cannot use multiple actions combined\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_INVALID_OPTION_COMBINATION);
    }

    vector<string> suffixes = {".png", ".PNG", ".jpg", ".JPG", ".JPEG", ".jpeg"};
    if (conf->needsDir || conf->needsArg) {
        while (optind < argc) {
            char *path = realpath(argv[optind], NULL);
            if (!path) {
                printf("Error: file/directory not found - %s\n", argv[optind]);
                printf("Try '%s -h' for more information\n", argv[0]);
                exit(ERROR_FILE_DOES_NOT_EXIST);
            }
            string str(path);
            struct stat s;
            if (stat(path, &s) != 0 ) {
                printf("Error: file/directory not found - %s\n", path);
                printf("Try '%s -h' for more information\n", argv[0]);
                exit(ERROR_FILE_DOES_NOT_EXIST);
            }
            if (s.st_mode & S_IFDIR) {
                    conf->dirnames.push_back(str);
            }
            else if (s.st_mode & S_IFREG) {
                if (conf->needsArg) {
                    int suffix_pos = str.find_last_of(".");
                    if (suffix_pos!=string::npos) {
                        if (find(suffixes.begin(), suffixes.end(), str.substr(suffix_pos))!=suffixes.end()) {
                            conf->filenames.push_back(str);
                        }
                    }
                    //conf->filenames.push_back(str);
                }
                else {
                    printf("Error: is not a directory - %s\n", path);
                    printf("Try '%s -h' for more information\n", argv[0]);
                    exit(ERROR_FILE_DOES_NOT_EXIST);
                }
            }
            else {
                printf("Error: is not a file/directory - %s\n", path);
                printf("Try '%s -h' for more information\n", argv[0]);
                exit(ERROR_FILE_DOES_NOT_EXIST);
            }
            free(path);
            optind++;
        }
    }


    if (conf->dirnames.size()==0 && conf->needsDir) {
        printf("Error: directory required\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_REQUIRED);
    }
    else if ((conf->filenames.size()==0 && conf->dirnames.size()==0) && conf->needsArg) {
        printf("Error: argument required\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_REQUIRED);
    }
    /*if ((conf->tags_to_search.size()) > 0 && conf->modifyTags) {
        printf("Error: cannot use option --filter-tag combined with option --add-tag or --remove-tag");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_INVALID_OPTION_COMBINATION);
    }
    if ((conf->needsFile || conf->needsDir) && conf->listTags) {
        printf("Error: cannot use option --add-tag, --remove-tag or --filter-tag combined with option --list-tag");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_INVALID_OPTION_COMBINATION);
    }
    if (conf->needsFile && conf->needsDir) {
        printf("Error: cannot use option --add-tag or --remove--tag combined with option --filter-tag");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_INVALID_OPTION_COMBINATION);
    }*/
    sort(conf->tags_to_add.begin(), conf->tags_to_add.end());
    sort(conf->tags_to_remove.begin(), conf->tags_to_remove.end());
    sort(conf->tags_to_search.begin(), conf->tags_to_search.end());
}

void printUsage(int argc, char* argv[]) {
    printf("Usage: %s [OPTIONS] [FILES | DIRECTORIES]\n", argv[0]);
    printf("\n");
    printf("    -h, --help                 Display this message\n");
    printf("\n");
    printf("Options:\n");
    printf("    -t, --add-tag TAG          Add tag to images (requires file argument)\n");
    printf("    -r, --remove-tag TAG       Remove tag from images (requires file argument)\n");
    printf("    -e, --exif                 Additionally write tags directly to the Exif data section\n");
    /*printf("    -a TAGS                  Add artist to image (requires file argument)\n");
    printf("    -n TAGS                    Remove artist from image (requires file argument)\n");
    printf("    -c TAGS                    Add character to image (requires file argument)\n");
    printf("    -e TAGS                    Remove character from image (requires file argument)\n");*/
    printf("    -l, --list-tags            List tags in database.\n");
    printf("    -f, --filter-tag TAG       Filter directories for images matching the tags (required directory argument)\n");
    printf("    --import-tags              Import tags from metadata (requires directory argument)\n");
    printf("    --export-tags              Export tags to metadata (requires directory argument)\n");
}
