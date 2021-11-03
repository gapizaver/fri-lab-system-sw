# include <stdio.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>

// program prebere n spodnjih vrstic datoteke
int main(int argc, char * argv [])
{
    if (argc > 3) {
        printf("Programu je potrebno podati največ 2 argumenta");
        exit(1);
    }
    if (argc < 2) {
        printf("Programu je potrebno podati 1 argument");
        exit(1);
    }

    int arg_filename = 1;
    int n = 5;  // število spodnjih vrstic za branje
    if (strlen(argv[1]) > 2 && argv[1][0] == '-' && argv[1][1] == 'n') {
        char* p = argv[1] + 2;
        n = atoi(p);
        arg_filename = 2;
    }

    int fd1 = open(argv[arg_filename], O_RDONLY);  // datoteka iz katere kopiramo
    if (fd1 < 0) {
        perror("Napaka");
        exit(1);
    }

    //off_t pos;
    off_t pos_back = -1;
    int line_counter = 0;

    // iskanje začetka branja datoteke
    while (line_counter < n) {
        if (!lseek(fd1, pos_back, SEEK_END)) {
            perror("Napaka");
            exit(1);
        }

        // branje prve datoteke
        char buff[1];
        if (!read(fd1, buff, 1)) {
            perror("Napaka");
            exit(1);
        }

        if (buff[0] == '\n'){
            line_counter++;
        }

        pos_back--;
    }

    // izpis zadnjih n vrstic na std izhod
    char buff[512];
    while ( (n = read(fd1, buff, 512) ) > 0) {
        if ( n < 0 ) { 
            perror("Napaka pri branju"); 
            exit(1); 
        }
        printf("%s", buff);
    }
    
    printf("\n");

    if (close(fd1) < 0){
        perror("Napaka pri zapiranju datoteke"); 
        exit(1);
    }

    return 0;
}