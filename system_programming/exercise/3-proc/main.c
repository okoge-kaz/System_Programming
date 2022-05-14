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

            // char *argv[] = {"whoami", NULL};
            int status1;
            fflush(stdout);
            pid_t pid1 = fork();
            if (pid1 == 0) {
                if (execvp(node->argv[0], node->argv) == -1) {
                    perror("execvp");
                    exit(errno);
                }
                status1 = execvp(node->argv[0], node->argv);
                exit(status1);
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

            break;

        case N_REDIRECT_IN:     /* foo < bar */
        case N_REDIRECT_OUT:    /* foo > bar */
        case N_REDIRECT_APPEND: /* foo >> bar */
            LOG("node->filename: %s", node->filename);

            /* Redirection (Task 4) */

            break;

        case N_SEQUENCE: /* foo ; bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            /* Sequential execution (Task 2) */
            int status2_1;
            fflush(stdout);
            pid_t pid2_1 = fork();
            if (pid2_1 == 0) {
                if (execvp(node->lhs->argv[0], node->lhs->argv) == -1) {
                    perror("execvp");
                    exit(errno);
                }
                status2_1 = execvp(node->lhs->argv[0], node->lhs->argv);
                exit(status2_1);
            }
            else if (pid2_1 == -1) {
                perror("fork");
                return errno;
            }
            else {
                waitpid(pid2_1, &status2_1, 0);
                
                int status2_2;
                fflush(stdout);
                pid_t pid2_2 = fork();
                if (pid2_2 == 0) {
                    if (execvp(node->rhs->argv[0], node->rhs->argv) == -1) {
                        perror("execvp");
                        exit(errno);
                    }
                    status2_2 = execvp(node->rhs->argv[0], node->rhs->argv);
                    exit(status2_2);
                }
                else if (pid2_2 == -1) {
                    perror("fork");
                    return errno;
                }
                else {
                    waitpid(pid2_2, &status2_2, 0);
                    return status2_2;
                }
            }

            break;

        case N_AND: /* foo && bar */
        case N_OR:  /* foo || bar */
            LOG("node->lhs: %s", inspect_node(node->lhs));
            LOG("node->rhs: %s", inspect_node(node->rhs));

            /* Branching (Task B) */

            break;

        case N_SUBSHELL: /* ( foo... ) */
            LOG("node->lhs: %s", inspect_node(node->lhs));

            /* Subshell execution (Task C) */

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
