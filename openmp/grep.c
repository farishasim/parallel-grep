#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include<sys/wait.h>

void test(int ID, int pid);

int main(int argc, char ** args) {

    pid_t * cpids;
    int NPROC;
    int NTHRD;
    char * STR;
    DIR * dir;
    
    if (argc < 4) {
        printf("Usage : ./grep n-process n-thread string\n");
        exit(0);
    }

    //*** INIT SECTION ***//    
    NPROC = atoi(args[1]);
    NTHRD = atoi(args[2]);
    STR = args[3];

    cpids = (pid_t*) malloc ( sizeof(pid_t) * NPROC );

    //*** PARENT SECTION ***//
    int i = 0;
    while(i < NPROC) {
        // parent process fork child process
        if ((cpids[0] = fork()) == 0) {
            // child process jump to child section
            goto child;
        } else {
            // parent process continue forking another child
            i++;
        }
    }

    // parent process exit when all child terminated
    while(wait(NULL) > 0);
    printf("Parent process ended\n");
    exit(0);

    //*** CHILD SECTION ***//
    child:
    omp_set_num_threads(NTHRD);
    #pragma omp parallel
    {
    int ID = omp_get_thread_num();
    // printf("hello(%d)", ID);
    // printf(" world(%d)\n", ID);
    test(ID, getpid());
    }
    
    return 0;
}

void test(int ID, int pid) {
    int ano = 1;
    printf("hello(%d, %d)", ID, pid);
    printf(" world(%d, %d)\n", ID, pid);
    return;
}