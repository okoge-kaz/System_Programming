#include "main.h"

#include <fcntl.h>     /* open */
#include <stdio.h>     /* fopen, fread, fwrite, fclose */
#include <stdlib.h>    /* exit, atoi */
#include <string.h>    /* strlen */
#include <sys/types.h> /* read */
#include <sys/uio.h>   /* read, write */
#include <unistd.h>    /* getopt, read, write, close */
/** argv[0] */
char *cmdname;

/* use stdio (-l option) */
int use_stdio = 0;

/* buffer size (-s N option) */
unsigned int buffer_size = 64;

int do_cat(int size, char **args) {
    for (int i = 0; i < size; i++) {
        char *fname = args[i];
        LOG("filename = %s", fname);
        if (strcmp(fname, "-") == 0) {
            if (use_stdio) {
                char buf[buffer_size];
                int n;
                while ((n = fread(buf, 1, buffer_size, stdin)) > 0) {
                    if(fwrite(buf, 1, n, stdout) != n) {
                        perror("fwrite");
                        exit(1);
                    }
                }
            } else {
                char buf[buffer_size];
                int n;
                while ((n = read(STDIN_FILENO, buf, buffer_size)) > 0) {
                    if(write(STDOUT_FILENO, buf, n) != n){
                        perror("write");
                        exit(1);
                    }
                }
            }
        } else {
            if (!use_stdio) {
                /* -l unavailable (Task 1); implement here */
                int file_discripter;
                if ((file_discripter = open(fname, O_RDONLY)) == -1) {
                    perror("open");
                    exit(1);
                }

                char *buffer = malloc(buffer_size);
                if (buffer == NULL) {
                    perror("malloc");
                    exit(1);
                }

                int n;
                while ((n = read(file_discripter, buffer, buffer_size)) > 0) {
                    if (write(STDOUT_FILENO, buffer, n) != n) {
                        perror("write");
                        exit(1);
                    }
                };

                free(buffer);
                if (close(file_discripter) == -1) {
                    perror("close");
                    exit(1);
                };
            } else {
                /* -l available (Task 2); implement here */
                FILE *file_pointer = fopen(fname, "r");
                if (file_pointer == NULL) {
                    perror("fopen");
                    exit(1);
                }

                char *buffer = malloc(buffer_size);
                if (buffer == NULL) {
                    perror("malloc");
                    exit(1);
                }

                int n;
                while ((n = fread(buffer, sizeof(char), buffer_size, file_pointer)) > 0) {
                    if (fwrite(buffer, sizeof(char), n, stdout) != n) {
                        perror("fwrite");
                        exit(1);
                    }
                };

                free(buffer);
                if (fclose(file_pointer) == EOF) {
                    perror("fclose");
                    exit(1);
                };
            }
        }
    }
    // すでに stdinに読み込まれているデータを stdout に書き出す処理が必要
    if (size == 0) {
        if (!use_stdio) {
            char *buffer = malloc(buffer_size);
            if (buffer == NULL) {
                perror("malloc");
                exit(1);
            }

            int n;
            while ((n = read(STDIN_FILENO, buffer, buffer_size)) > 0) {
                if (write(STDOUT_FILENO, buffer, n) != n) {
                    perror("write");
                    exit(1);
                }
            };

            free(buffer);
        } else {
            char *buffer = malloc(buffer_size);
            if (buffer == NULL) {
                perror("malloc");
                exit(1);
            }

            int n;
            while ((n = fread(buffer, sizeof(char), buffer_size, stdin)) > 0) {
                if (fwrite(buffer, sizeof(char), n, stdout) != n) {
                    perror("fwrite");
                    exit(1);
                }
            };
            free(buffer);
        }
    }
    return 0;
}

/** parse program arguments. */
void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qls:")) != -1) {
        switch (opt) {
            case 'q': /* -q: quiet */
                l_set_quiet(1);
                break;
            case 'l': /* -l: use stdio */
                use_stdio = 1;
                break;
            case 's': /* -s N: set buffer size */
                buffer_size = atoi(optarg);
                if (buffer_size == 0) {
                    DIE("Invalid size");
                }
                break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s [-q] [-l] [-s size] file ...\n",
                        cmdname);
                exit(EXIT_FAILURE);
        }
    }
}

/** the entrypoint */
int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);
    return do_cat(argc - optind, argv + optind);
}
