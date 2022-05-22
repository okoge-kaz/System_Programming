#include <signal.h>
#include <stdio.h>

static volatile long long c1 = 0, c2 = 0;

void handler(int sig) {
    c1++;
    c2++;
}

int main() {
    signal(SIGINT, handler);
    for (long long i = 0; i < 5000000000; i++) {
        c1++;
        c2++;
    }
    printf("c1`s value is %lld\n", c1);
    printf("c2`s value is %lld\n", c2);
}