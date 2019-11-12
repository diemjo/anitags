#include "../inc/error_codes.h"
#include "../inc/config.h"
#include "../inc/command_line.h"

using namespace std;

void processArgs(int argc, char* argv[], config* conf) {
    int c;
    struct option options[] = {
            {"add-tag", required_argument, 0, 't'},
            {"remove-tag", required_argument, 0, 'r'},
            {"filter-tag", required_argument, 0, 'f'},
            {"exif", required_argument, 0, 'e'},
            {"help", required_argument, 0, 'h'}
    };
    int option_index = 0;
    conf->needsFile=true;
    while ((c = getopt_long(argc, argv, "t:r:f:ehl", options, &option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 't':
                conf->tags_to_add.push_back(optarg);
                conf->needsFile=true;
                conf->needsDir=false;
                conf->modifyTags=true;
                break;
            case 'r':
                conf->tags_to_remove.push_back(optarg);
                conf->needsFile=true;
                conf->needsDir=false;
                conf->modifyTags=true;
                break;
            case 'f':
                conf->tags_to_search.push_back(optarg);
                conf->searchByTags=true;
                conf->needsDir=true;
                conf->needsFile=false;
                break;
            case 'e':
                conf->modifyExif=true;
                break;
            case 'l':
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

    if (conf->needsDir || conf->needsFile) {
        while (optind < argc) {
            char *path = realpath(argv[optind], NULL);
            if (path) {
                string str(path);
                struct stat s;
                if (stat(path, &s) == 0 ) {
                    if (s.st_mode & S_IFDIR) {
                        if (conf->needsDir) {
                            conf->dirnames.push_back(str);
                        }
                        else {
                            printf("Error: is not a file - %s\n", path);
                            printf("Try '%s -h' for more information\n", argv[0]);
                            exit(ERROR_FILE_DOES_NOT_EXIST);
                        }
                    } else if (s.st_mode & S_IFREG) {
                        if (conf->needsFile) {
                            conf->filenames.push_back(str);
                        }
                        else {
                            printf("Error: is not a file - %s\n", path);
                            printf("Try '%s -h' for more information\n", argv[0]);
                            exit(ERROR_FILE_DOES_NOT_EXIST);
                        }
                    } else {
                        printf("Error: is not a file/directory - %s\n", path);
                        printf("Try '%s -h' for more information\n", argv[0]);
                        exit(ERROR_FILE_DOES_NOT_EXIST);
                    }
                }
                else {
                    printf("Error: file/directory not found - %s\n", path);
                    printf("Try '%s -h' for more information\n", argv[0]);
                    exit(ERROR_FILE_DOES_NOT_EXIST);
                }
            }
            else {
                printf("Error: file/directory not found - %s\n", argv[optind]);
                printf("Try '%s -h' for more information\n", argv[0]);
                exit(ERROR_FILE_DOES_NOT_EXIST);
            }
            free(path);
            optind++;
        }
    }


    if (conf->filenames.size()==0 && conf->needsFile) {
        printf("Error: file required\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_REQUIRED);
    }
    else if (conf->dirnames.size()==0 && conf->needsDir) {
        printf("Error: directory required\n");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_FILE_REQUIRED);
    }
    if ((conf->tags_to_search.size()) > 0 && conf->modifyTags) {
        printf("Error: cannot use option '-s' combined with option '-t' or '-r'");
        printf("Try '%s -h' for more information\n", argv[0]);
        exit(ERROR_OPTION_S_WITH_T_R);
    }
    sort(conf->tags_to_add.begin(), conf->tags_to_add.end());
    sort(conf->tags_to_remove.begin(), conf->tags_to_remove.end());
    sort(conf->tags_to_search.begin(), conf->tags_to_search.end());
}

void printUsage(int argc, char* argv[]) {
    printf("Usage: %s [OPTIONS] [FILES]\n", argv[0]);
    printf("\n");
    printf("    -h, --help                 Display this message\n");
    printf("\n");
    printf("Options:\n");
    printf("    -t, --add-tag TAG          Add tag to images (requires file argument)\n");
    printf("    -r, --remove-tag TAG       Remove tag from images (requires file argument)\n");
    /*printf("    -a TAGS                  Add artist to image (requires file argument)\n");
    printf("    -n TAGS                    Remove artist from image (requires file argument)\n");
    printf("    -c TAGS                    Add character to image (requires file argument)\n");
    printf("    -e TAGS                    Remove character from image (requires file argument)\n");*/
    printf("    -l, --list                 List tags of images (requires file argument). default on.\n");
    printf("    -f, --filter-tag TAG       Filter directories for images matching the tags\n");
}
