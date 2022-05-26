#include "main.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h> /* kill, signal */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/** The max characters of one cmdline (incl. NULL terminator) */
#define MAX_LINE 8192

#define EMOJI_BIRD "\xF0\x9F\x90\xA6"
#define EMOJI_FIRE "\xF0\x9F\x94\xA5"

char *fire = EMOJI_FIRE "\n";
char *prompt = "ttsh[%d] " EMOJI_BIRD " ";

/** Whether or not to show the prompt */
int show_prompt = 1;

/** Checks whether two given strings are the same. */
bool streql(const char *lhs, const char *rhs) {
    return (lhs == NULL && rhs == NULL) ||
           (lhs != NULL && rhs != NULL && strcmp(lhs, rhs) == 0);
}

/** Erase line break characters */
char *chomp(char *line) {
    char *p = strchr(line, '\n');
    if (p) *p = '\0';
    return line;
}

pid_t stop_target_pid;
pid_t stopped_target_pid;
const int SIG_STOP = -(1LL << 30);

void sigint_handler(int signum) {
    pid_t pid;
    int status;

    switch (signum) {
        case SIGINT:
            LOG("SIGINT received\n");
            printf("\n");
            show_prompt = 1;
            break;
        case SIGTSTP:
            if(stop_target_pid == SIG_STOP) break;
            LOG("SIGTSTP received\n");
            kill(stop_target_pid, SIGTSTP);
            stopped_target_pid = stop_target_pid;
            stop_target_pid = SIG_STOP;
            break;
        case SIGCHLD:
            LOG("SIGCHLD received\n");
            while ((pid = waitpid(-1, &status, WNOHANG)) != 0) {
                if (pid == -1) {
                    if (errno == ECHILD) break;    // no child processes
                    if (errno == EINTR) continue;  // interrupted by signal
                    exit(1);                       // error
                }
                if (WIFEXITED(status)) {
                    int result = write(STDERR_FILENO, fire, strlen(fire));
                    if (result == -1) PERROR_DIE("write");
                }
            }
            break;
        default:
            LOG("Unknown signal received\n");
            break;
    }
    return;
}

/** Run a node and obtain an exit status. */
int invoke_node(node_t *node) {
    LOG("Invoke: %s", inspect_node(node));
    pid_t pid;

    if (strcmp(node->argv[0], "fg") == 0) {
        if (node->argv[1] == NULL) {
            kill(stopped_target_pid, SIGCONT);
            return 0;
        } else {
            perror("fg: too many arguments");
            exit(1);
        }
    }

    if (strcmp(node->argv[0], "bg") == 0) {
        if (node->argv[1] == NULL) {
            signal(SIGCHLD, sigint_handler);
            pid = fork();
            if (pid == 0) {
                kill(stopped_target_pid, SIGCONT);
                exit(0);
            } else if (pid == -1) {
                perror("fork");
                exit(1);
            }
            return 0;
        } else {
            perror("bg: too many arguments");
            exit(1);
        }
    }

    // Checks whether the command is executed with '&'
    if (node->async) {
        LOG("{&} found: async execution required");
        signal(SIGCHLD, sigint_handler);
        pid = fork();
        if (pid == 0) {
            if (execvp(node->argv[0], node->argv) == -1) PERROR_DIE("execvp");
            return 0; /* never happen */
        } else if (pid == -1) {
            PERROR_DIE("fork");
        } else {
            return 0;
        }
    }

    // generates a child process
    fflush(stdout);
    pid = fork();
    if (pid == -1) PERROR_DIE("fork");

    if (pid == 0) {
        // child
        stop_target_pid = getpid();
        if (execvp(node->argv[0], node->argv) == -1) PERROR_DIE("execvp");
        return 0; /* never happen */
    }

    // assign an independent process group
    if (setpgid(pid, pid) == -1) PERROR_DIE("setpgid");

    // wait a child process
    signal(SIGTSTP, sigint_handler);

    int status;
    int options = WUNTRACED;
    pid_t waited_pid = waitpid(pid, &status, options);
    if (waited_pid == -1) {
        if (errno != ECHILD) PERROR_DIE("waitpid");
    } else {
        LOG("Waited: target_pid=%d, raw_stat=%d, stat=%d", waited_pid, status, WEXITSTATUS(status));
        if (WIFEXITED(status)) {
            int result = write(STDERR_FILENO, fire, strlen(fire));
            if (result == -1) PERROR_DIE("write");
        }
    }
    return WEXITSTATUS(status);
}

void parse_options(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "qp")) != -1) {
        switch (opt) {
            case 'q': /* -q: quiet */
                l_set_quiet(1);
                break;
            case 'p': /* -p: no-prompt */
                show_prompt = 0;
                break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s [-q] [-p] [cmdline ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (getenv("NO_EMOJI") != NULL) {
        fire = "FIRE\n";
        prompt = "ttsh[%d]> ";
    }
}

int invoke_line(char *line) {
    LOG("Input line='%s'", line);
    node_t *node = yacc_parse(line);
    if (node == NULL) {
        LOG("Obtained empty line: ignored");
        return 0;
    }
    int exit_status = invoke_node(node);
    free_node(node);
    return exit_status;
}

int main(int argc, char **argv) {
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
        if (show_prompt) {
            printf(prompt, history_id);
        }
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return EXIT_SUCCESS;  // EOF: Ctrl-D (^D)
        }
        invoke_line(chomp(line));
    }
}