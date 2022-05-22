#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
int main(int argc, char **argv) {
    int fd[2], st1, st2;
    char * cmd1_argv[2], * cmd2_argv[2];
    cmd1_argv[0] = argv[1];
    cmd2_argv[0] = argv[2];
    cmd1_argv[1] = cmd2_argv[1] = NULL;
    pipe(fd);
    int pid1 = fork();
    if (pid1 == 0) {
        dup2(fd[1], 1);
        close(fd[0]);
        close(fd[1]);
        execvp(cmd1_argv[0], cmd1_argv); 
        printf("execvp failed\n");
        return 1;
    }
    close(fd[1]);
    int pid2 = fork();
    if (pid2 == 0) {
        dup2(fd[0], 0);
        close(fd[0]);
        execvp(cmd2_argv[0], cmd2_argv); 
        printf("execvp failed\n");
        return 1;
    }
    close(fd[0]);
    wait(&st1);
    wait(&st2);

    printf("%d %d\n", WEXITSTATUS(st1), WEXITSTATUS(st2));
    printf("%d %d\n", WIFEXITED(st1), WIFEXITED(st2));

    if(WIFEXITED(st1) && WIFEXITED(st2) && (WEXITSTATUS(st1) & WEXITSTATUS(st2)) == 0) {
        printf("%s\n", "Success");
    } else {
        printf("%s\n", "Failure");
    }
    return 0;
}