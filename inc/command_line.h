//
// Created by yo on 11/8/19.
//

#ifndef ANITAGS_COMMAND_LINE_H
#define ANITAGS_COMMAND_LINE_H

#include <stdio.h>
#include <string>
#include <vector>
#include <getopt.h>
#include <sys/stat.h>
#include <algorithm>

#include "config.h"

void printUsage(int argc, char* argv[]);
void processArgs(int argc, char* argv[], config* conf);

#endif //ANITAGS_COMMAND_LINE_H
