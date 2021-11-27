#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct mymsg {
	long mtype;
	char mtext[512];
};

int main(int argc, char const *argv[])
{
    // odpri datoteko podano v arg za pisanje prejetih sporočil
    if (argc < 2) {
        printf("Potrebujem argument: pot do datoteke.\n");
        exit(1);
    }
    FILE* fp;
    fp = fopen(argv[1], "w");

    // pridobi identifikator sporočilne vrste
    key_t msg_key;
    int msg;
    struct mymsg buf; 
    if ((msg_key = ftok("/tmp", 'C')) < 0) {
        perror("Napaka pri ustvarjanju ključa");
        exit(1);
    }
    if ((msg = msgget(msg_key, 0644)) < 0) {
        perror("Napaka pri pridobivanju identifikatorja sporočilne vrste");
        exit(1);
    }

    // branje sporočil dokler ne prejmeš prazno sporočilo
    int writtenb = 2;
    do {
        // prejmi sporočilo
        if (msgrcv(msg, &buf, sizeof(buf.mtext), 0, 0) < 0) {
            perror("Napaka pri sprejemanju sporočila iz sporočilne vrste");
            exit(1);
        }

        // zapiši sporočilo v datoteko
        writtenb = fprintf(fp, "%s", buf.mtext);
        if (writtenb < 0) {
            perror("Napaka pri pisanju v datoteko");
            exit(1);
        }
    } while (writtenb > 1);

    // zapri datoteko
    fclose(fp);

    // uniči sporočilno vrsto
    if (msgctl(msg, IPC_RMID, NULL) < 0) {
        perror("Napaka pri brisanju sporočilne vrste");
        exit(1);
    }

    return 0;
}
