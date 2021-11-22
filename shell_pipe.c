#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BUF_SIZE 1024

int parser(char* p, char** arg, char** arg2);

void sig_int(int signo) {
    if (alarm(3) == 0)
        printf("\nZa izhod iz programa se enkrat pritisnite CTRL+C v roku 3 sekund.\n");
    else {
        exit(0);
    }
}

void sig_alrm(int signo) {
    printf("Ukaz: ");
    fflush(stdout);
}

int main(int argc, char const *argv[])
{
    // ujemi singal SIG_INT
    if (signal(SIGINT, sig_int) == SIG_ERR)
        perror("signal error");

    // ujemi signal SIG_ALRM
    if (signal(SIGALRM, sig_alrm) == SIG_ERR)
        perror("Sgnal error");
    
    while (1) {
        char* buffer;
        size_t bufsize = 128;
        char* args[bufsize];
        char* args2[bufsize];
        size_t characters;
        pid_t pid;
        int status;

        buffer = (char *)malloc(bufsize * sizeof(char));
        if( buffer == NULL)
        {
            perror("Unable to allocate buffer");
            exit(1);
        }
        
        // preberi ukaz
        printf("Ukaz: ");
        int pipe_used = getline(&buffer, &bufsize, stdin);

        buffer[strlen(buffer) - 1] = 0;
        parser(buffer, args, args2);

        // bodoči stari starš ustvari otroka
        if ( (pid = fork()) < 0)
			perror("fork starega starša error");
		else if (pid == 0) {
            if (pipe_used) {
                // ustvari cev za komunikacijo med otrokom in vnukom
                int fd[2];
                char buf[BUF_SIZE];
                if (pipe(fd) < 0) {
                    perror("Napaka pri ustvarjanju cevi");
                    exit(1);
                }

                // usvari novega otroka (vnuka)
                pid_t pid2;
                if ((pid2 = fork()) < 0)
                    perror("fork otroka error");

                else if (pid2 == 0) {
                    // vnuk zapre bralni del cevi in preusmeri stdout na pisalni del
                    close(fd[0]);
                    dup2(fd[1], 1);
                    close(fd[1]);

                    // vnuk izved program na desnem delu cevi
                    execvp(args[0], args);
                    printf("Vnuk ne more izvesti ukaza %s\n", args2[0]);
                    perror("Vnuk error");
                    exit(127);
                } else {
                    // otrok zapre pisalni del cevi in preusmeri stdin na bralni del
                    close(fd[1]);
                    dup2(fd[0], 0);
                    close(fd[0]);

                    // otrok počaka vnuka
                    if ((pid2 = waitpid(pid2, &status, 0) < 0))
                        perror("waitpid error, čakanje vnuka");
                }
            }

            // otrok izvede program na levi strani cevi
            execvp(args2[0], args2);
            printf("Otrok ne more izvesti ukaza %s\n", args[0]);
            exit(127);
		} else {
            // stari starš počaka otroka
            if ( (pid = waitpid(pid, &status, 0)) < 0)
                perror("waitpid error, čakanje otroka");

            free(buffer);
        }
    }

    return 0;
}

int parser(char* p, char** arg, char** arg2) {
    int poln = 0;
    int pipe = 0;
	
	while (*p != 0)
	{
		if ( *p == ' ' || *p == '\t') {
			poln = 0;
			*p++ = 0;
		}
        else if (*p == '-' && *(p+1) == '>') {
            // znak za pipe "->"
            poln = 0;
            pipe = 1;
            *arg++ = NULL;
            p++;
            p++;
        }
		else {
			if (!poln) {
                if (pipe == 1) {
                    *arg2++ = p;
                } else 
                    *arg++ = p;
            }
            
			poln = 1;
			p++;
		}
	}

    *arg = (char *)0;
    *arg2 = (char *)0;

    return pipe;
}


