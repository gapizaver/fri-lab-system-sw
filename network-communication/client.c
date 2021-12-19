#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_SIZE 128

int main(int argc, char const *argv[])
{
    // socket
    int fd;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(1);
    }

    // connect
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6969);
    const char addr[] = "0.0.0.0";
    inet_pton(AF_INET, "0.0.0.0", &servaddr.sin_addr);
    if (0 > (connect(fd, (struct sockaddr*) &servaddr, sizeof(servaddr)))) {
        perror("Connect error");
        exit(1);
    }

    char buffer[BUF_SIZE];
    int n;
    while (1) {
        // read stdio until EOF
        if ((n = read(STDIN_FILENO, buffer, BUF_SIZE)) < 0) {
            perror("Read error");
            exit(1);
        } else if (n == 0) {
            break;
        }
        buffer[BUF_SIZE -1] = 0;

        // write buffer to server
        if (write(fd, buffer, BUF_SIZE) < 0) {
            perror("Write error");
            exit(1);
        }

        // sprejmi rezultat (v obliki double)
        if (read(fd, buffer, BUF_SIZE) != BUF_SIZE) {
            perror("Read error");
            exit(1);
        }

        printf("%s\n", buffer);
    }

    // zapri povezavo
    close(fd);

    return 0;
}
