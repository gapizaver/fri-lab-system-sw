# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>

int main(int argc, char * argv [])
{
    if (argc != 3) {
        printf("Programu je potrebno podati 2 argumenta");
        exit(1);
    }

    int fd1 = open(argv[1], O_RDONLY);  // datoteka iz katere kopiramo
    int fd2 = open(argv[2], O_WRONLY);  // datoteka v katero kopiramo

    if (fd1 < 0 || fd2 < 0) {
        perror("Napaka");
        exit(1);
    }

    //off_t pos;
    off_t pos_back = -1;

    while (lseek(fd1, pos_back, SEEK_END) >= 0) {
        char buff[1];

        // branje prve datoteke
        if (!read(fd1, buff, 1)) {
            perror("Napaka");
            exit(1);
        }

        // pisanje v drugo datoteko
        if (!write(fd2, buff, 1)) {
            perror("Napaka");
            exit(1);
        }

        pos_back--;
    }

    if (close(fd1) < 0){
        perror("Napaka pri zapiranju datoteke"); 
        exit(1);
    }   
    if (close(fd2) < 0){
        perror("Napaka pri zapiranju datoteke"); 
        exit(1);
    }   
 
    return 0;
}