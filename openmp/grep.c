#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>

void test(int ID);

#define BLOCK 4096

int NPROC;
int NTHRD;

int main(int argc, char ** args) {

    char * STR;
    DIR * dir;
    char buffer[BLOCK];
    char * filename;
    
    if (argc < 4) {
        printf("Usage : ./grep n-process n-thread string\n");
        exit(0);
    }

    //*** INIT SECTION ***//    
    NPROC = atoi(args[1]);
    NTHRD = atoi(args[2]);
    STR = args[3];

    omp_set_nested(1);
    omp_set_num_threads(NPROC);
    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf("this is from process(%d): ", ID);

        test(ID);
    }
}

void test(int ID) {
    omp_set_num_threads(NTHRD);
    #pragma omp parallel
    {
    printf("hello(%d)", ID);
    printf(" world(%d)\n", ID);
    #pragma omp barrier
    #pragma omp single
    printf("All thread is : %d\n", omp_get_num_threads());
    }
    return;
}