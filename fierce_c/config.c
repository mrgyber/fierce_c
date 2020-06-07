#include "config.h"

void help() {
    printf("Syntax: fierce_c [options]\n");
    printf("OPTIONS:\n");
    printf("\t-help:    print help\n");
    printf("\t-host:    hostname\n");
    printf("\t-dns:     dns server ip (1.1.1.1 by default)\n");
    printf("\t-file:    file path with prefixes\n");
    printf("\t-threads: number of threads\n");
    printf("\t-timeout: timeout for each request\n");
    printf("\n");
    printf("EXAMPLE:\n");
    printf("\tfierce_c -host example.ru\n");
    printf("\tfierce_c -host example.ru -dns 8.8.8.8 -file dict/5000.txt -threads 8 -timeout 0.1\n");
}

/**
 * Fill the config with values from argv
 */
void config_prepare(int argc, char **argv) {
    config = (CONFIG *) malloc(sizeof(CONFIG));
    config->host = NULL;
    config->dns = NULL;
    config->file = NULL;
    config->threads = 0;
    config->timeout = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-help") == 0) {
            help();
            exit(0);
        }
        else if(strcmp(argv[i], "-host") == 0) {
            config->host = (char *) argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "-file") == 0) {
            config->file = (char *) argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "-dns") == 0) {
            config->dns = (char *) argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "-threads") == 0) {
            char *end;
            long int threads = strtol(argv[i + 1], &end, 10);
            config->threads = threads;
            i++;
        }
        else if(strcmp(argv[i], "-timeout") == 0) {
            double timeout = strtof(argv[i + 1], NULL);
            config->timeout = timeout;
            i++;
        }
    }
}

/**
 * Check the config for required parameters and set default values if something is not found
 */
int config_check() {
    if (config->host == NULL) {
        help();
        printf("\nYou need to specify -host parameter\n");
        return 0;
    }
    if (config->dns == NULL) {
        config->dns = "1.1.1.1";
    }
    if (config->threads == 0) {
        config->threads = 8;
    }
    if (config->timeout == 0) {
        config->timeout = 0.1;
    }
    return 1;
}
