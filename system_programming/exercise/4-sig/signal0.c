#include <signal.h> /* signal */
#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* write */
volatile int always_true;

void handler1(int sig) {
    signal(SIGINT, handler1);
    return;
}

void handler2(int sig) {
    signal(SIGALRM, handler2);
    fprintf(stderr, "*");
    alarm(5);
}

int main() {
    /* Exercise 1: Implement here. */
    signal(SIGINT, handler1);
    /* Exercise 2: Implement here. */
    alarm(5);
    signal(SIGALRM, handler2);

    /* Prints dots. */
    always_true = rand() >= 0;
    for (;;) {
        for (int j = 0; j < 1024 * 1024 * 500; j++)
            if (!always_true) break; /* busy loop */
        int result = write(STDERR_FILENO, ".", 1);
        if (result == -1) exit(1);
    }
    return 0;
}
