#define _GNU_SOURCE
#include <dlfcn.h>  /* dladdr */
#include <stdarg.h> /* va_list */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* atexit, getenv */
#include <string.h> /* strcmp */

#define MAX_DEPTH 32
#define MAX_CALLS 1024

__attribute__((no_instrument_function)) int log_to_stderr(const char *file, int line, const char *func, const char *format, ...) {
    char message[4096];
    va_list va;
    va_start(va, format);
    vsprintf(message, format, va);
    va_end(va);
    return fprintf(stderr, "%s:%d(%s): %s\n", file, line, func, message);
}
#define LOG(...) log_to_stderr(__FILE__, __LINE__, __func__, __VA_ARGS__)

__attribute__((no_instrument_function))
const char *
addr2name(void *address) {
    Dl_info dli;
    if (dladdr(address, &dli) == 0) {
        return NULL;
    }
    return dli.dli_sname;
}
int seen = 0;
char *branches[MAX_CALLS];
int cnt[MAX_CALLS];
int i = 0;

int is_new(const char *name) {
    for (int j = 0; j < i; j++) {
        if (strcmp(name, branches[j]) == 0) {
            cnt[j]++;
            return 0;
        }
    }
    cnt[i] = 1;
    return 1;
}

void final_bracket(void) {
    char *label = getenv("SYSPROG_LABEL");

    FILE *ifp = fopen("cg.dot", "r");
    char buf[1024];
    char *result[MAX_CALLS * MAX_DEPTH];
    int index = 0;

    while (fgets(buf, sizeof(buf), ifp) != NULL) {
        if (strstr(buf, "}") != NULL) {
            // } があった
            fclose(ifp);
            FILE *ofp = fopen("cg.dot", "w");
            for (int k = 0; k < index; k++) {
                fprintf(ofp, "%s", result[k]);
            }
            // fprintf(ofp, "}\n");
            fclose(ofp);
            break;
        }
        if (strstr(buf, "__cxa_finalize_ranges -> final_bracket") != NULL) continue;
        result[index] = strdup(buf);
        index++;
    }
    fclose(ifp);

    FILE *f = fopen("cg.dot", "w");
    if (f != NULL) {
        for (int k = 0; k < index; k++) {
            fprintf(f, "%s", result[k]);
        }
        fprintf(f, "}\n");
        fclose(f);
    }
    if (label != NULL) {
        // 発展課題A
        FILE *fp = fopen("cg", "r");
        if (fp != NULL) {
            char line[1024];
            while (fgets(line, sizeof(line), fp) != NULL) {
                if (is_new(line) == 1) {
                    branches[i] = strdup(line);
                    i++;
                }
            }
            fclose(fp);
        }
        FILE *f = fopen("cg.dot", "w");
        fprintf(f, "strict digraph G {\n");
        for (int j = 0; j < i; j++) {
            fprintf(f, "%s\" [label=\"%d\"]\n", branches[j], cnt[j]);
        }
        fprintf(f, "}\n");
        fclose(f);
    }
    return;
}

__attribute__((no_instrument_function)) void __cyg_profile_func_enter(void *addr, void *call_site) {
    /* Not Yet Implemented */
    LOG("LOG start\n");
    LOG("addr %s (%p)\n", addr2name(addr), addr);
    LOG("call %s (%p)\n", addr2name(call_site), call_site);
    LOG("LOG end\n");

    if (strcmp(addr2name(addr), "main") == 0) {
        // main関数での呼び出し
        LOG("call_site is null\n");
        FILE *f = fopen("cg.dot", "w");
        if (f != NULL) {
            fprintf(f, "strict digraph G {\n");
            // fprintf(f, "%s -> %s\n", addr2name(call_site), addr2name(addr));
            fclose(f);
        }
    } else {
        if (strcmp(addr2name(call_site), "main") == 0 && seen == 0) {
            // main関数からの呼び出し
            LOG("call_site is main\n");
            seen = 1;
            atexit(final_bracket);
            FILE *f = fopen("cg.dot", "a");
            if (f != NULL) {
                fprintf(f, "%s -> %s\n", addr2name(call_site), addr2name(addr));
                fclose(f);
            }
        } else {
            // 書き足し
            FILE *f = fopen("cg.dot", "a");
            if (f != NULL) {
                fprintf(f, "%s -> %s\n", addr2name(call_site), addr2name(addr));
                fclose(f);
            }
        }
    }
}

__attribute__((no_instrument_function)) void __cyg_profile_func_exit(void *addr, void *call_site) {
    /* Not Yet Implemented */
    LOG("LOG start\n");
    LOG("addr %s (%p)\n", addr2name(addr), addr);
    LOG("call %s (%p)\n", addr2name(call_site), call_site);
    LOG("LOG end\n");

    char *label = getenv("SYSPROG_LABEL");
    if (label != NULL) {
        // 発展課題A
        return;
    }
    if (strcmp(addr2name(addr), "main") == 0) {
        FILE *f = fopen("cg.dot", "a");
        if (f != NULL) {
            fprintf(f, "}\n");
            fclose(f);
        }
    }
}
