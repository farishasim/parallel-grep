#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** args) {

    pid_t * cpids;
    int NPROC;
    int NTHRD;
    char * STR;
    
    if (argc < 4) {
        printf("Usage : ./grep n-process n-thread string\n");
        exit(0);
    }

    //*** INIT SECTION ***//    
    NPROC = atoi(args[1]);
    NTHRD = atoi(args[2]);
    STR = args[3];

    cpids = (pid_t*) malloc ( sizeof(pid_t) * NPROC );

    // if ((cpid = fork()) == 0) {
    //     printf("Hello from child");
    // } else {
    //     printf("Hello from parent");
    // }

    omp_set_num_threads(NTHRD);
    #pragma omp parallel
    {
    int ID = omp_get_thread_num();
    printf("hello(%d)", ID);
    printf(" world(%d)\n", ID);
    }
    
    return 0;
}