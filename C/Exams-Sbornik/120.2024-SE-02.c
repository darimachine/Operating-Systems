#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}
int pids[10]={-2};

int handleSingleProgram(const char* program) {
    pid_t pid = wrapper_fork();
    if(pid==0) {

        execlp(program,program,(char*)NULL);
        err(12,"Error execing");
    }

    return pid;
    
}
int findIndex(pid_t pid) {
    for(int i=0;i<10;i++) {
        if(pids[i] == pid) {
            return i;
        }
    }
    return -1;
}
int main(int argc, char* argv[]) {

    if(argc <2 || argc>11) {
        errx(3,"error invalid arguments");
    }
    for(int i=1;i<argc;i++) {
        pid_t pid = handleSingleProgram(argv[i]);
        pids[i-1]=pid;
    }

    while(true) {
        int status;
        pid_t pid = wait(&status);
        if(pid<0) {
            break;
        }
        if(!WIFEXITED(status)) {
            int index = findIndex(pid);
            pids[index] =-2;
            
            printf("IWANT TO SEE: %d\n",index);
            printf("Trqva da e -2 %d\n",pids[index]);
            for(int i=0;i<argc-1;i++){
                if(pids[i]!=-2) {
                    printf("Trqbva da e razlichni ot -2 %d\n",pids[i]);
                    printf("Killing %d\n",i);
                    if(kill(pids[i],SIGTERM) <0) {
                        err(32,"Error on kill");
                    }
                }
            }
            int exitNumber = index + 1;
            exit(exitNumber);
        }
        if(WEXITSTATUS(status) ==0) {
            int index = findIndex(pid);
            pids[index] = -2;
        }
        else if(WEXITSTATUS(status)!=0) {
                int index = findIndex(pid);
                pid_t newPid = handleSingleProgram(argv[index+1]);
                pids[index] = newPid;
        }
    }
}
