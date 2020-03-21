#pragma once

#include <pthread.h>

#include "config.h"
#include "dns_request.h"

extern unsigned int __5000_txt_len;
extern unsigned char __5000_txt[];

void * default_thread(void *data);
void * file_thread(void * data);
int file_len();
void run_threads();
void intHandler(int dummy);
