#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));// initialize to 0

    addr.sin_family = AF_INET;
    addr.sin_port = htons(10000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(fd, 5);

    for(;;){
        int client_fd = accept(fd, NULL, NULL);
        int n;
        char buf[1024];
        while((n = read(client_fd, buf, sizeof(buf))) > 0){
            write(client_fd, buf, n);
        }
        close(client_fd);
    }
    return 0;
}