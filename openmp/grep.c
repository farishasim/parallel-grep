#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

void test(int ID);

#define BLOCK 4096
#define NORMAL_COLOR  "\x1B[0m"
#define GREEN  "\x1B[32m"
#define BLUE  "\x1B[34m"

int NPROC;
int NTHRD;

// List definition
typedef int fd;
typedef struct tNode* ptr;
typedef struct tNode {
    fd info;    // file descriptor
    ptr next;   // next pointer
} Node ;
typedef ptr List;

ptr AlokNode(fd info) {
    ptr P = (ptr) malloc (sizeof(Node));
    P->info = info;
    P->next = NULL;
    return P;
}

ptr last(List L) {
    ptr P = L;
    while (P->next) P = P->next;
    return P;
}

List listing(char * path) {
    List L = NULL; ptr P = AlokNode(0); // dummy node
    DIR * d = opendir(path);
    if (d==NULL) return L;
    struct dirent * dir;
    char d_path[512];
    while ((dir = readdir(d)) != NULL) {
        sprintf(d_path, "%s/%s", path, dir->d_name);
        printf("%s\n", d_path);
        if (dir->d_type != DT_DIR) // for files
        {
            P->next = AlokNode(fileno(fopen(d_path, "r")));
            P = P->next;
            if (!L) L = P;
        }
        else if ((dir->d_type == DT_DIR) // for directory
                && (strcmp(dir->d_name,".") != 0) 
                && (strcmp(dir->d_name,"..") != 0))
        {
            P->next = listing(d_path);
            if (P->next){
                P = last(P->next);
                if (!L) L = P->next;
            }
        }
    }
    closedir(d);
    return L;
}

int main(int argc, char ** args) {

    char * STR;
    DIR * dir;
    char buffer[BLOCK];
    char * filename;
    
    if (argc < 4) {
        printf("Usage : ./grep n-process n-thread string\n");
        exit(0);
    }

    List L = listing(".");

    //*** INIT SECTION ***//    
    NPROC = atoi(args[1]);
    NTHRD = atoi(args[2]);
    STR = args[3];

    // parallelism for file search

}

void test(int ID) {
    // parallelism for string search in a file
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