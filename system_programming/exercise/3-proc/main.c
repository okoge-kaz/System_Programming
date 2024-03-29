#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/** The max characters of one cmdline (incl. NULL terminator) */
#define MAX_LINE 8192

/** The max number of pipes that can occur in one cmdline */
#define MAX_PIPE 16

#define BIRD "\xF0\x9F\x90\xA6"

/** Whether or not to show the prompt */
int prompt = 1;

char *cmdname;

/** Run a node and obtain an exit status. */
int invoke_node(node_t *node) {
    /* You can modify any part of the body of this function.
       Adding new functions is, of course, also allowed. */

    LOG("Invoke: %s", inspect_node(node));

    switch (node->type) {
        case N_COMMAND:
            /* change directory (cd with no argument is not supported.) */
            LOG("Command: %s", inspect_node(node));

            if (strcmp(node->argv[0], "cd") == 0) {
                if (node->argv[1] == NULL) {
                    return 0;  // do nothing
                } else if (chdir(node->argv[1]) == -1) {
                    perror("cd");
                    return errno;
                } else {
                    return 0;
                }
            }

            /* Simple command execution (Task 1) */
            int status1;
            fflush(stdout);
            pid_t pid1 = fork();
            if (pid1 == 0) {
                status1 = execvp(node->argv[0], node->argv);
                if (status1 == -1) {
                    perror("execvp");
                    exit(errno);
                }
            } else if (pid1 == -1) {
                perror("fork");
                return errno;
            } else {
                waitpid(pid1, &status1, 0);
                return status1;
            }
            break;

        case N_PIPE: /* foo | bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            /* Pipe execution (Tasks 3 and A) */
            int file_discriptor[2];
            int status3_1 = pipe(file_discriptor);
            if (status3_1 == -1) {
                perror("pipe");
                return errno;
            }
            int status3_2;
            fflush(stdout);
            pid_t pid3_1 = fork();
            if (pid3_1 == 0) {
                // child process1
                dup2(file_discriptor[1], 1);
                close(file_discriptor[0]);
                close(file_discriptor[1]);
                if (node->lhs->lhs == NULL) {
                    status3_2 = execvp(node->lhs->argv[0], node->lhs->argv);
                    if (status3_2 == -1) {
                        perror("execvp");
                        exit(errno);
                    }
                } else {
                    status3_2 = invoke_node(node->lhs);
                    if (status3_2 != 0) {
                        exit(status3_2);
                    }
                    _exit(status3_2);
                }
            } else if (pid3_1 == -1) {
                perror("fork");
                return errno;
            } else {
                // parent process
                fflush(stdout);
                pid_t pid3_2 = fork();
                if (pid3_2 == 0) {
                    // child process2
                    dup2(file_discriptor[0], 0);
                    close(file_discriptor[0]);
                    close(file_discriptor[1]);
                    if (node->rhs->lhs == NULL) {
                        status3_2 = execvp(node->rhs->argv[0], node->rhs->argv);
                        if (status3_2 == -1) {
                            perror("execvp");
                            exit(errno);
                        }
                    } else {
                        status3_2 = invoke_node(node->rhs);
                        if (status3_2 == -1) {
                            perror("invoke_node");
                            exit(errno);
                        }
                        _exit(status3_2);
                    }
                } else if (pid3_2 == -1) {
                    perror("fork");
                    return errno;
                } else {
                    close(file_discriptor[0]);
                    close(file_discriptor[1]);

                    waitpid(pid3_1, &status3_1, 0);
                    waitpid(pid3_2, &status3_2, 0);
                    return status3_2;
                }
            }
            break;

        case N_REDIRECT_IN: /* foo < bar */
            LOG("node->filename: %s", node->filename);
            LOG("node->lhs: %s", inspect_node(node->lhs));

            int status4_IN;
            fflush(stdout);
            pid_t pid4_IN = fork();
            if (pid4_IN == 0) {
                int fd = open(node->filename, O_RDONLY);
                if (fd == -1) {
                    perror("open");
                    exit(errno);
                }
                dup2(fd, 0);
                close(fd);

                if (node->lhs->lhs == NULL) {
                    status4_IN = execvp(node->lhs->argv[0], node->lhs->argv);
                    if (status4_IN == -1) {
                        perror("execvp");
                        exit(errno);
                    }
                } else {
                    status4_IN = invoke_node(node->lhs);
                    if (status4_IN == -1) {
                        perror("invoke_node");
                        exit(errno);
                    }
                    _exit(status4_IN);
                }
            } else if (pid4_IN == -1) {
                perror("fork");
                return errno;
            } else {
                waitpid(pid4_IN, &status4_IN, 0);
                return status4_IN;
            }
            break;
        case N_REDIRECT_OUT: /* foo > bar */
            LOG("node->filename: %s", node->filename);
            LOG("node->lhs: %s", inspect_node(node->lhs));

            int status4_OUT;
            fflush(stdout);
            pid_t pid4_OUT = fork();
            if (pid4_OUT == 0) {
                int fd = open(node->filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1) {
                    perror("open");
                    exit(errno);
                }
                dup2(fd, 1);
                close(fd);

                if (node->lhs->lhs == NULL) {
                    status4_OUT = execvp(node->lhs->argv[0], node->lhs->argv);
                    if (status4_OUT == -1) {
                        perror("execvp");
                        exit(errno);
                    }
                } else {
                    status4_OUT = invoke_node(node->lhs);
                    if (status4_OUT == -1) {
                        perror("invoke_node");
                        exit(errno);
                    }
                    _exit(status4_OUT);
                }
            } else if (pid4_OUT == -1) {
                perror("fork");
                return errno;
            } else {
                waitpid(pid4_OUT, &status4_OUT, 0);
                return status4_OUT;
            }
            break;

        case N_REDIRECT_APPEND: /* foo >> bar */
            LOG("node->filename: %s", node->filename);
            LOG("node->lhs: %s", inspect_node(node->lhs));

            /* Redirection (Task 4) */
            int status4_APPEND;
            fflush(stdout);
            pid_t pid4 = fork();
            if (pid4 == 0) {
                int fd = open(node->filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
                if (fd == -1) {
                    perror("open");
                    exit(errno);
                }
                dup2(fd, 1);
                close(fd);

                if (node->lhs->lhs == NULL) {
                    status4_APPEND = execvp(node->lhs->argv[0], node->lhs->argv);
                    if (status4_APPEND == -1) {
                        perror("execvp");
                        exit(errno);
                    }
                } else {
                    status4_APPEND = invoke_node(node->lhs);
                    if (status4_APPEND == -1) {
                        perror("invoke_node");
                        exit(errno);
                    }
                    _exit(status4_APPEND);
                }
            } else if (pid4 == -1) {
                perror("fork");
                return errno;
            } else {
                waitpid(pid4, &status4_APPEND, 0);
                return status4_APPEND;
            }
            break;

        case N_SEQUENCE: /* foo ; bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            /* Sequential execution (Task 2) */
            int status2;
            status2 = invoke_node(node->lhs);
            status2 = invoke_node(node->rhs);

            return status2;

            break;

        case N_AND: /* foo && bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            int statusB_AND;
            statusB_AND = invoke_node(node->lhs);
            if (statusB_AND == 0) {
                statusB_AND = invoke_node(node->rhs);
            }
            return statusB_AND;

            break;

        case N_OR: /* foo || bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            /* Branching (Task B) */
            int statusB_OR;
            statusB_OR = invoke_node(node->lhs);
            if (statusB_OR != 0) {
                statusB_OR = invoke_node(node->rhs);
            }
            return statusB_OR;

            break;

        case N_SUBSHELL: /* ( foo... ) */
            LOG("node->lhs: %s", inspect_node(node->lhs));

            /* Subshell execution (Task C) */
            int statusC;
            fflush(stdout);
            pid_t pidC = fork();
            if (pidC == 0) {
                statusC = invoke_node(node->lhs);
                _exit(statusC);
            } else if (pidC == -1) {
                perror("fork");
                return errno;
            } else {
                waitpid(pidC, &statusC, 0);
                return statusC;
            }
            break;

        default:
            assert(false);
    }
    return 0;
}

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qp")) != -1) {
        switch (opt) {
            case 'q': /* -q: quiet */
                l_set_quiet(1);
                break;
            case 'p': /* -p: no-prompt */
                prompt = 0;
                break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s [-q] [-p] [cmdline ...]\n", cmdname);
                exit(EXIT_FAILURE);
        }
    }
}

int invoke_line(char *line) {
    LOG("Input line='%s'", line);
    node_t *node = yacc_parse(line);
    if (node == NULL) {
        LOG("Obtained empty line: ignored");
        return 0;
    }
    if (!l_get_quiet()) {
        dump_node(node, stdout);
    }
    int exit_status = invoke_node(node);
    free_node(node);
    return exit_status;
}

int main(int argc, char **argv) {
    cmdname = argv[0];
    parse_options(argc, argv);
    if (optind < argc) {
        /* Execute each cmdline in the arguments if exists */
        int exit_status;
        for (int i = optind; i < argc; i++) {
            exit_status = invoke_line(argv[i]);
        }
        return exit_status;
    }

    for (int history_id = 1;; history_id++) {
        char line[MAX_LINE];
        if (prompt) {
            // printf("ttsh[%d]> ", history_id);
            printf("ttsh[%d] %s ", history_id, BIRD);
        }
        /* Read one line */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF: Ctrl-D (^D) */
            return EXIT_SUCCESS;
        }
        /* Erase line breaks */
        char *p = strchr(line, '\n');
        if (p) {
            *p = '\0';
        }
        invoke_line(line);
    }
}
