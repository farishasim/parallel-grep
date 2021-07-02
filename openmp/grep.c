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

#define FILESIZE 4096
#define BLOCK 256

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
    List L = NULL;
    DIR * d = opendir(path);
    if (d==NULL) return L;
    struct dirent * dir; ptr P = AlokNode(0), Pinit = P; // dummy node
    char d_path[512];
    while ((dir = readdir(d)) != NULL) {
        sprintf(d_path, "%s/%s", path, dir->d_name);
        if (dir->d_type != DT_DIR) // for files
        {
            printf("%s\n", d_path);
            P->next = AlokNode(fileno(fopen(d_path, "r")));
            P = P->next;
            if (!L) L = P;
        }
        else if ((dir->d_type == DT_DIR) // for directory
                && (strcmp(dir->d_name,".") != 0) 
                && (strcmp(dir->d_name,"..") != 0))
        {
            P->next = listing(d_path); // recursive call
            if (P->next) {
                P = last(P->next);
                if (!L) L = P->next;
            }
        }
    }
    free(Pinit);
    closedir(d);
    return L;
}

int searchFile(char * text, char * pattern, int length) {
    // length : of text
    int i,j;
    for(i = 0; i< length; i++) {
        for(j = 0; pattern[j] != '\0'; j++) {
            if (text[i+j] != pattern[j]) break;
        }
        if (pattern[j] == '\0') return 1; // true
    }
    return 0; // false
}

int main(int argc, char ** args) {

    char * STR;
    DIR * dir;
    char buffer[FILESIZE];
    char * filename;
    ptr P; List L;
    int child;
    fd no; 
    
    if (argc < 4) {
        printf("Usage : ./grep n-process n-thread string\n");
        exit(0);
    }

    L = listing(".");

    //*** INIT SECTION ***//    
    NPROC = atoi(args[1]);
    NTHRD = atoi(args[2]);
    STR = args[3];

    //*** PARENT SECTION ***//
    P = L; child = 0;
    while (P) {
        pid_t cid;
        if ((cid = fork()) == 0) {
            // child process jump to child section
            goto child;
        }
        // parent process continue forking another child
        child++;
        if (child == NPROC) {
            int stat;
            cid = wait(&stat);
            child--;
        }
        P = P->next;
    }

    while(child > 0) {
        int stat; pid_t cid;
        cid = wait(&stat);
        child--;
    }
    
    exit(0);
    
    //*** CHILD SECTION ***//
    child:
    no = P->info;
    FILE *fp = fdopen(no, "rb");
    fread(buffer, 1, FILESIZE, fp);
    if (searchFile(buffer, "omp.h", FILESIZE)) {
        char filePath[BLOCK];
        char result[BLOCK];
        sprintf(filePath, "/proc/self/fd/%d", no);
        memset(result, 0, sizeof(result));
        readlink(filePath, result, sizeof(result));
        printf("%s\n", result);
    }

    return 0;
}

// void test(int ID) {
//     // parallelism for string search in a file
//     omp_set_num_threads(NTHRD);
//     #pragma omp parallel
//     {
//     printf("hello(%d)", ID);
//     printf(" world(%d)\n", ID);
//     #pragma omp barrier
//     #pragma omp single
//     printf("All thread is : %d\n", omp_get_num_threads());
//     }
//     return;
// }