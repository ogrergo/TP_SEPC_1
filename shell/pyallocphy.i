%module pyallocphy
%{
#define SWIG_FILE_WITH_INIT
#include "../src/mem.h"
%}
int mem_init();
void *mem_alloc(unsigned long size);
int mem_free(void *ptr, unsigned long size);
int mem_destroy();
