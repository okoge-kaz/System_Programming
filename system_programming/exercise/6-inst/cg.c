#define _GNU_SOURCE
#include <dlfcn.h>  /* dladdr */
#include <stdarg.h> /* va_list */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* atexit, getenv */

#define MAX_DEPTH 32
#define MAX_CALLS 1024

__attribute__((no_instrument_function))
int log_to_stderr(const char *file, int line, const char *func, const char *format, ...) {
    char message[4096];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    return fprintf(stderr, "%s:%d(%s): %s\n", file, line, func, message);
}
#define LOG(...) log_to_stderr(__FILE__, __LINE__, __func__, __VA_ARGS__)

__attribute__((no_instrument_function))
const char *addr2name(void* address) {
    Dl_info dli;
    if (dladdr(address, &dli) == 0) {
        return NULL;
    }
    return dli.dli_sname;
}


__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *addr, void *call_site) {
    /* Not Yet Implemented */
    
    /* You are encouraged to remove this default stupid implementation and
       write code from scratch. */
    FILE *f = fopen("cg.dot", "w");
    if (f != NULL) {
        fprintf(f, "strict digraph G {\n");
        fprintf(f, "  L -> hoge -> \"<\";\n");
        fprintf(f, "  \"' - '\" -> hoge;\n");
        fprintf(f, "  R -> hoge -> \">\";\n");
        fprintf(f, "}\n");
        fclose(f);
    }
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *call_site) {
    /* Not Yet Implemented */
}
