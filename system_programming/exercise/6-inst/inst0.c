#define _GNU_SOURCE
#include <dlfcn.h> /* dladdr */
#include <stdio.h> /* printf */

__attribute__((no_instrument_function))
const char *addr2name(void* address) {
    Dl_info dli;
    return dladdr(address, &dli) != 0 ? dli.dli_sname : NULL;
}

__attribute__((no_instrument_function))
void __cyg_profile_func_enter(void *addr, void *site_call) {
    // printf(">>> %s (%p)\n", addr2name(addr), addr);
}

__attribute__((no_instrument_function))
void __cyg_profile_func_exit(void *addr, void *site_call) {
    // printf("<<< %s (%p)\n", addr2name(addr), addr);
}

void foo(void);
void bar(void);
void baz(void);
void qux(void);

int main(void) {
    printf("main: %p\n", main);
    foo();
    foo();
    return 0;
}

void foo(void) {
    printf("main -> foo: %p\n", foo);
    bar();
    baz();
}

void bar(void) {
    printf("main -> foo -> bar: %p\n", bar);
}

void baz(void) {
    printf("main -> foo -> baz: %p\n", baz);
}

void qux(void) {
    printf("qux?\n");
}
