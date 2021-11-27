#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

struct mymsg {
    long mtype;
    char mtext[512];
};

int main(int argc, char const *argv[])
{
    // ustvari novo sporočilno vrsto
    key_t key_msg = ftok("/tmp", 'C');
    int msg;
    if ((msg = msgget(key_msg, 0644 | IPC_CREAT)) < 0) {
        perror("Napaka pri ustvarjanju sporočilne vrste");
        exit(1);
    }

    // ustvari struct mymsg
    struct mymsg msg_buf;
    msg_buf.mtype = 8;

    // pridobi identifikator semaforja
    key_t key_sem = ftok("/tmp", 'A');
    int sem;
    if ((sem = semget( key_sem, 1, 0664)) < 0) {
        perror("Napaka pri ustvarjanju semaforja na posredniku");
        exit(1);
    }

    // pripravi operacijo za semafor
    struct sembuf operations[1];
    operations[0].sem_num = 0; // kateri semafor
    operations[0].sem_flg = 0; // zastavica (čakanje)

    // pridobi identifikator za pomnilnik
    key_t key_shm = ftok("/tmp", 'B');
    int shm;
    if ((shm = shmget(key_shm, 1024, 0664)) < 0) {
        perror("Napaka pri ustvarjanju deljenega pomnilnika na posredniku");
        exit(1);
    }

    // priklop na shm
    char* shmp;
    shmp = (char *)shmat(shm, (void *)0, 0);
    if (shmp == (char *)(-1)) {
        perror("Napaka pri priklopu na shm v generatorju");
        exit(1);
    }

    // rezerviraj prostor za buffer prejetega sporočila
    char* buf = (char*) malloc(1024*sizeof(char));

    // na 1 sekundo dostopaj do deljenega pomnilnika in preberi vsebino
    do {
        // poskušaj pridobiti dostop do shm in ga zakleni
        operations[0].sem_op = -1;
        if (semop(sem, operations, 1) < 0) {
            perror("Napaka pri zaklepanju semaforja v posredniku");
            exit(1);
        }
        
        // preberi sporočilo iz shm
        int new;
        if (strcmp(buf, shmp) == 0) {
            new = 0;
        } else {
            strcpy(buf, shmp);
            new = 1;
        }

        // sprosti deljneni pomnilnik (sem postavi na 1)
        operations[0].sem_op = 1; // katera operacija
        if (semop(sem, operations, 1) < 0) {
            perror("Napaka pri sproščanju semaforja v posredniku");
            exit(1);
        }

        if (new) {
            printf("Prejeto novo sporočilo: %s\n", shmp);

            // skopiraj prejeto v sporočilo za sporočilno vrsto
            strcpy(msg_buf.mtext, shmp);
            // pošlji sporočilo v sporočilno vrsto
            if (msgsnd(msg, &msg_buf, strlen(msg_buf.mtext)+1, 0) < 0) {
                perror("Napaka pri pošiljanju v sporočilno vrsto");
                exit(1);
            }
        } else {
            printf("Prejeto enako sporočilo\n");
        }

        // počakaj 1s pred ponovnim branjem
        sleep(1);
    } while (strlen(buf) > 0);

    // odklop z deljenega pomnilnika
    if (shmdt(shmp) < 0) {
        perror("Napaka pri odklopu z shm v posredniku");
        exit(1);
    }

    // brisanje deljenega pomnilnika
    if (shmctl(shm, IPC_RMID, NULL) < 0) {
        perror("Napaka pri brisanju deljenega pomn. v posredniku");
        exit(1);
    }

    // uničenje semaforja
    union semun sem_arg;
    if (semctl(sem, 1, IPC_RMID, sem_arg) < 0) {
        perror("Napaka pri brisanju semaforja v posredniku");
        exit(1);
    }
    
    return 0;
}
