# include <stdio.h>
# include <stdlib.h>
# include <fcntl.h>
# include <sys/types.h>
# include <dirent.h>
# include <sys/stat.h>
# include <string.h>

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        printf("Program potrebuje 2 argumenta.\n");
        exit(1);
    }

    struct stat buf;
    if (stat(argv[2], &buf) == -1) {
        perror("Napaka pri branju datoteke:");
        exit(1);
    }
    mode_t mode_buf = buf.st_mode;

    char* ep; // kazalec na prvi znak, ki ni število
    long int new_perm = strtol(argv[1], &ep, 8);

    // določi tip podanega argumenta
    if (ep == argv[1]) {
        if (strlen(argv[1]) != 3) {
            printf("Napačna raba argumenta za podajo pravic");
            exit(1);
        }

        if (argv[1][0] == 'u') {
            if (argv[1][1] == '+') {
                mode_buf |= S_ISUID;
            } else if (argv[1][1] == '-') {
                mode_buf &= ~S_ISUID;
            } else {
                printf("Napačna raba argumenta za podajo pravic");
                exit(1);
            }
        } else if (argv[1][0] == 'g') {
            if (argv[1][1] == '+') {
                mode_buf |= S_ISGID;
            } else if (argv[1][1] == '-') {
                mode_buf &= ~S_ISGID;
            } else {
                printf("Napačna raba argumenta za podajo pravic");
                exit(1);
            }
        } else {
            printf("Napačna raba argumenta za podajo pravic");
            exit(1);
        }
    } else {
        if (strlen(argv[1]) != 4) {
            printf("Napačna raba argumenta za podajo pravic");
            exit(1);
        }

        mode_buf = new_perm;
    }

    // uvedi spremembe dovoljenj
    if (chmod(argv[2], mode_buf) != 0) {
        perror("Napaka pri uveljavljanju novih pravic.");
        exit(1);
    }

    return 0;
}
