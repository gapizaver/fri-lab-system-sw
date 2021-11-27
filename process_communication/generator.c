#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main(int argc, char const *argv[])
{
    // ustvari semafor
    key_t key_sem = ftok("/tmp", 'A');
    int sem;
    if ((sem = semget( key_sem, 1, 0664 | IPC_CREAT)) < 0) {
        perror("Napaka pri ustvarjanju semaforja na generatorju:");
        exit(1);
    }

    // nastavi semafor na 1
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(sem, 0, SETVAL, sem_arg) < 0) {
        perror("Napaka pri postavljanju semaforja na 1 v generatorju");
        exit(1);
    }

    // operacija za semafor
    struct sembuf operations[1];
    operations[0].sem_num = 0; // kateri semafor
    operations[0].sem_flg = 0; // zastavica (čakanje)

    // ustvari deljeni pomnilnik
    key_t key_shm = ftok("/tmp", 'B');
    int shm;
    if ((shm = shmget(key_shm, 1024, 0664 | IPC_CREAT)) < 0) {
        perror("Napaka pri ustvarjanju deljenega pomnilnika na generatorju");
        exit(1);
    }
    
    // priklop na shm
    char* shmp;
    shmp = (char *)shmat(shm, (void *)0, 0);
    if (shmp == (char *)(-1)) {
        perror("Napaka pri priklopu na shm v generatorju");
        exit(1);
    }

    // zapiši 5 sporočil v shm
    for (size_t i = 0; i < 5; i++)
    {
        // s sem zakleni deljeni pomnilnik (sem postavi na 0)
        operations[0].sem_op = -1; // katera operacija
        if (semop(sem, operations, 1) < 0) {
            perror("Napaka pri zaklepanju semaforja v generatorju");
            exit(1);
        }
        
        // zapiši sporočilo na shm
        printf("Zapisujem sporočilo %ld\n", i+1);
        if (i == 4)
            shmp[0] = '\0';
        else
            sprintf(shmp, "ID %ld: To je sporočilo.", i+1);

        // drži shm zakljenjen 2s (za demonstracijo)
        sleep(2);

        // sprosti deljneni pomnilnik (sem postavi na 1)
        operations[0].sem_op = 1; // katera operacija
        if (semop(sem, operations, 1) < 0) {
            perror("Napaka pri sproščanju semaforja v generatorju");
            exit(1);
        }

        // počakaj 2s pred pošiljanjem novega sporočila
        sleep(2);
    }

    // odklop z deljenega pomnilnika
    if (shmdt(shmp) < 0) {
        perror("Napaka pri odklopu z shm v generatorju");
        exit(1);
    }

    return 0;
}
