#include <signal.h> /* signal */
#include <stdlib.h> /* exit */
#include <unistd.h> /* write */

volatile int always_true;

int main() {
    /* Exercise 1: Implement here. */

    /* Exercise 2: Implement here. */
    // alarm(5);

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
