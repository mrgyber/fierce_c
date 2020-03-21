#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct __config__ {
    char * host;
    char * dns;
    char * file;
    long int threads;
    double timeout;
} CONFIG;

CONFIG * config;

void help();
void config_prepare(int argc, char **argv);
int config_check();
