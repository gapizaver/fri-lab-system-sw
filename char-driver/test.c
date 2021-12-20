#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 64

int main(int argc, char const *argv[])
{
    // odpri modul
    int fd;
    if ((fd = open("/dev/mydevice", O_RDONLY)) < 0) {
        perror("open");
        exit(1);
    }

    // preberi sporočilo
    char buf[BUF_SIZE];
    int n;
    if ((n = read(fd, &buf, BUF_SIZE)) < 0) {
        perror("read");
        exit(1);
    }
    printf("%s", buf);
    fflush(stdout);

    // zapri modul
    if (close(fd) < 0) {
        perror("close");
        exit(1);
    }
    return 0;
}
