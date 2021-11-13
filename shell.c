#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 128

void parser(char* p, char** args);

int main(int argc, char const *argv[])
{
    while (1) {
        char* buffer;
        size_t bufsize = 128;
        char* args[bufsize];
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
        getline(&buffer, &bufsize, stdin);

        buffer[strlen(buffer) - 1] = 0;

        parser(buffer, args);

        if ( (pid = fork()) < 0)
			perror("fork error");
		else if (pid == 0) {
            // otrok izvede program
			execvp( args[0], args);
			printf("Ne morem izvesti ukaza %s\n", buffer);
			exit(127);
		}
		// starš
		if ( (pid = waitpid(pid, &status, 0)) < 0)
			perror("waitpid error");
        free(buffer);
    }

    return 0;
}

void parser(char* p, char** arg) {
    int poln = 0;
	
	while (*p != 0)
	{
		if ( *p == ' ' || *p == '\t') {
			poln = 0;
			*p++ = 0;
		}
		else {
			if ( !poln)
				*arg++ = p;
			poln = 1;
			p++;
		}
	}
	*arg = (char *)0;
}

