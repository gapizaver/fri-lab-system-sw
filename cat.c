# include <stdio.h>
# include <fcntl.h>

int main(int argc, char * argv [])
{
    int fd;
    if (argc < 2) {
        fd = 1;
    } else {
        fd = open(argv[1], O_RDONLY); 
    }

    if (fd < 0) {
        perror("Napaka: ");
        return -1;
    }

    int i = 1;
    do {
        int n;
        char buf[512];
        while ( (n = read(fd, buf, 512) ) > 0) {
            if ( write(1, buf, n) != n ) { 
                perror("Napaka pri prisanju: "); 
                exit(1); 
            }
        }

        if (n < 0) {
            perror("Napaka pri branju: ");
        }

        if (close(fd) < 0) {
            perror("Napaka pri zapiranju: ");
        }

        if (++i < argc) {
            fd = open(argv[i], O_RDONLY);
        }
    } while (i < argc);

    return 0;
}