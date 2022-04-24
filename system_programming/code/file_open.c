#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int fd1 = open("tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    int fd2 = open("tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd1, "ABCD", 4);
    write(fd2, "ab", 2);
    close(fd1);
    close(fd2);
}