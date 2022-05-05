#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    int n;
    char buf[1024];

    pipe(fd);                             // create a pipe: fd[0] for reading and fd[1] for writing
    write(fd[1], "Hello, world!\n", 14);  // write to fd[1]
    n = read(fd[0], buf, 1024);           // read from fd[0]

    write(STDOUT_FILENO, buf, n);  // write to STDOUT
    close(fd[0]);                  // close fd[0]
    close(fd[1]);                  // close fd[1]
}