#include <stdio.h>  /* fprintf */
#include <string.h> /* strcmp */
#include <stdlib.h> /* atoi, strcmp */
#include <unistd.h> /* sleep, usleep */
#include <time.h>   /* nanosleep, clock_nanosleep */

#define EMOJI_ZOMBIE "\xF0\x9F\xA7\x9F"
#define PLAIN_ZOMBIE "Z..."

int sleep_halfsec(void) {
    // sleep(1);
    // usleep(500 * 1000);
    struct timespec ts = { 0, 500 * 1000 * 1000 };
#ifdef __linux
    // WSL1 issue: new glibc uses CLOCK_REALTIME although WSL1 doesn't implement it
    // @see https://qiita.com/mmns/items/eaf42dd3345a2285ff9e
    return clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL);
#else
    return nanosleep(&ts, NULL);
#endif
}

int main(int argc, char **argv) {
    int print_all = 0;

    if (argc < 2) {
        fprintf(stderr, "usage: %s [-a] <num> [label]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_index = 1;
    if (strcmp(argv[1], "-a") == 0) {
        print_all = 1;
        num_index = 2;
    }
    int sec = atoi(argv[num_index]);
    for (int i = sec; i > 0; i--) {
        if (print_all || i <= 3) {
            if (argc > num_index + 1) {
                fprintf(stderr, "%s-%d ", argv[num_index + 1], i);
            } else {
                fprintf(stderr, "%d ", i);
            }
        }
        sleep_halfsec();
    }
    int use_emoji = getenv("NO_EMOJI") == NULL;
    fprintf(stderr, "%s ", use_emoji ? EMOJI_ZOMBIE : PLAIN_ZOMBIE);
    return 0;
}
