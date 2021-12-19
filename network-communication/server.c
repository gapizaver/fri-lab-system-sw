#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define BUF_SIZE 128

int sockfd;

// signal SIGINT -> zapri povezavo
void sig_int(int signo) {
    close(sockfd);
    exit(0);
}

int main(int argc, char const *argv[])
{
    // ujemi singal SIG_INT
    if (signal(SIGINT, sig_int) == SIG_ERR)
        perror("signal error");

    // socket
    sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(1);
    }
    
    // bind
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;                      // ipv4
    servaddr.sin_port = htons(6969);                    // port
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);       // ip
    if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        perror("Bind error");
        exit(1);
    }
    
    // listen
    if (0 > listen(sockfd, SOMAXCONN)) {
        perror("Listen error");
        exit(1);
    }
    printf("Poslušam na naslovu %d na vratih %d\n", servaddr.sin_addr.s_addr, servaddr.sin_port);

    // sprejemaj odjemalce
    while (1) {
        char buf[BUF_SIZE];
        int fd;
        if (0 > (fd = accept(sockfd, NULL, NULL))) {
            perror("Accept error");
            exit(1);
        }

        // preberi odjemalca
        int n;
        while (0 < (n = read(fd, buf, BUF_SIZE))) {
            if (n < 0) {
                perror("Read error");
                exit(1);
            } else if (n == 0) {
                // EOF
                break;
            }

            // izračunaj odjemalčev račun
            double a;
            double b;
            char operator;
            double result;

            // če nepravilen format preskoči vnos
            if (sscanf(buf, "%lf %c %lf", &a, &operator, &b) != 3) {
                printf("Incorrect format\n");
                continue;
            }

            // izračun
            switch (operator)
            {
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;           
            case '/':
                result = a / b;
                break;
            }

            // pošlji rezultat nazaj
            snprintf(buf, BUF_SIZE, "%f", result);            // pretvori double -> string
            if (write(fd, buf, BUF_SIZE) < 0) {
                perror("Write error");
                exit(1);
            }
        }
    }

    return 0;
}