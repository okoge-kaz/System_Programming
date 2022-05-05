#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int main(void) {
    int file_descriptor[2];
    int n;

    char buffer[1024];
    pipe(file_descriptor);  // create a pipe: file_descriptor[0] for reading and file_descriptor[1] for writing

    if (fork() == 0) {
        // child process
        close(file_descriptor[1]);
        n = read(file_descriptor[0], buffer, 14);  // read from file_descriptor[0]
        write(STDOUT_FILENO, buffer, n);           // write to STDOUT
        close(file_descriptor[0]);
    } else {
        // parent process
        close(file_descriptor[0]);
        write(file_descriptor[1], "Hello, world!\n", 14);  // write to file_descriptor[1]
        close(file_descriptor[1]);
    }
}