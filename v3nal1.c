# include <stdio.h>
# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>

int main(int argc, char * argv [])
{
    if (argc < 2) {
        // generiraj binarno datoteko "stevila"
        int fd = open("stevila", O_WRONLY | O_CREAT | O_TRUNC, 0777);
        if (fd < 0) {
            perror("Napaka: ");
            return -1;
        }

        int buff[] = {5};
        if ( write(fd, buff, sizeof(int)) != sizeof(int) ) { 
            perror("Napaka pri prisanju: "); 
            exit(1); 
        }

        float buff1[] = {1.1, 2.2, 3.3, 4.4, 5.5};
        if ( write(fd, buff1, 5*sizeof(float)) != 5*sizeof(float)) { 
            perror("Napaka pri prisanju: "); 
            exit(1); 
        }

        if (close(fd) < 0){
            perror("Napaka pri zapiranju datoteke"); 
            exit(1);
        }
    } else {
        int fd = open("stevila", O_RDONLY);
        if (fd < 0) {
            perror("Napaka: ");
            return -1;
        }
        
        int n;
        if ((read(fd, &n, sizeof(int))) < 0) {
            perror("Napaka pri branju:"); 
            exit(1);
        }

        printf("%d\n", n);

        float *p = (float *) calloc(n, sizeof(float));
        if ((read(fd, p, n*sizeof(float))) < 0) {
            perror("Napaka pri branju:"); 
            exit(1);
        }

        for (size_t i = 0; i < n; i++)
        {
            printf("%f\n", p[i]);
        }

        free(p); 
    }

    return 0;
}