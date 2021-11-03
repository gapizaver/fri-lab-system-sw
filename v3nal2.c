# include <stdio.h>
# include <fcntl.h>
# include <stdlib.h>
# include <time.h>

int main(int argc, char * argv [])
{
    FILE* fp = fopen("izhod1", "w");
    FILE* fp2 = fopen("izhod2", "w");

    if (!fp || !fp2) {
        perror("Napaka:");
        exit(1);
    }

    int vbuff;
    char buff[100];
    if ((vbuff = setvbuf(fp, buff, _IOFBF, 10)) != 0) {
        perror("Napaka buffer:");
        exit(1);
    }

    time_t seconds1;
    seconds1 = time(NULL);
    for (size_t i = 0; i < 100000; i++)
    {
        fprintf(fp, "To je moj najljubši stavek.");
    }
    printf("zapisovanje1: %lds\n", time(NULL) - seconds1);
    
    if ((vbuff = setvbuf(fp, buff, _IOFBF, 100)) != 0) {
        perror("Napaka buffer:");
        exit(1);
    }

    seconds1 = time(NULL);
    for (size_t i = 0; i < 100000; i++)
    {
        fprintf(fp2, "To je moj najljubši stavek.");
    }
    printf("zapisovanje2: %lds\n", time(NULL) - seconds1);

    return 0;
}