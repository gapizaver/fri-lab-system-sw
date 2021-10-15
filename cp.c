# include <stdio.h>
# include <fcntl.h>

int main(int argc, char * argv [])
{
    if (argc != 3) {
        fprintf(stderr, "Potrebujem 2 argumenta");
        return -1;
    }

    int fd1 = open(argv[1], O_RDONLY );
    if (fd1 < 0) {
        perror("Napaka: ");
        return -1;
    }

    int fd2 = open(argv[2], O_WRONLY | O_CREAT );
    if (fd2 < 0) {
        perror("Napaka: ");
        return -1;
    }

    int n;
    char buf[512];
    while ( (n = read(fd1, buf, 512) ) > 0) {
        if ( write(fd2, buf, n) != n ) { 
            perror("Napaka pri prisanju: "); 
            exit(1); 
        }
    }

    if (n < 0) {
        perror("Napaka pri branju:");
    }

    return 0;
}