#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define N_THREADS 8

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

size_t n_skupaj = 4;            // vse kapljice
size_t z_skupaj = 3;            // kapljice znotraj kroga

void* threadWork() {
    // naštimaj seed za 
    struct timespec myTime;
    clock_gettime(CLOCK_REALTIME,&myTime); // program prevedi s stikalom -lrt
    unsigned int seed = myTime.tv_nsec;

    while (n_skupaj < 1000000000) {
        size_t n = 0;       // vse kapljice niti
        size_t z = 0;       // kapljice znotraj kroga
        while (n < 1000000) {
            double x = ((double) rand_r(&seed)) / RAND_MAX;   
            double y = ((double) rand_r(&seed)) / RAND_MAX;
            
            n++;
            // če kapljica znotraj kroga
            if (x*x + y*y <= 1)
                z++;
        }

        // povečaj glabalni spremenljivki
        pthread_mutex_lock(&mymutex);
        n_skupaj += n;
        z_skupaj += z;
        pthread_mutex_unlock(&mymutex);
    }
    
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_t id;
    
    for (size_t i = 0; i < N_THREADS; i++) {
        if ( pthread_create(&id, NULL, threadWork, NULL) ) {
            perror("Thread create");
        }
    }

    if (pthread_join(id, NULL)) {
        perror("Thread join");
        exit(1);
    }

    // izpiši rezultat
    printf("Izračunan Pi:%f\n", (4.0*z_skupaj)/n_skupaj );
    printf("dejanski PI:%f\n", M_PI);

    return 0;
}
