#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void) {
    int fd[2];

    static char buffer[1024 * 1024];
    pipe(fd);                              // create a pipe: fd[0] for reading and fd[1] for writing
    write(fd[1], buffer, sizeof(buffer));  // write to fd[1]
    read(fd[0], buffer, sizeof(buffer));   // read from fd[0]

    close(fd[0]);  // close fd[0]
    close(fd[1]);  // close fd[1]

    // bufferのサイズよりも大きいデータが入ると詰まる
}