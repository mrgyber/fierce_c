#include <signal.h>

#include "config.h"
#include "threads.h"

int main(int argc, char **argv) {

    signal(SIGINT, intHandler); // In threads.c

    config_prepare(argc, argv);
    if (!config_check()) return 0;

    printf("DNS Servers for %s:\n", config->host);
    dns_request(config->host, config->dns, T_NS, 0);

    run_threads();

    return 0;
}
