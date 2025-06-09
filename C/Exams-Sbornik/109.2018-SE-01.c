#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


int wrapper_wait(void) {
    int status;
    if(wait(&status)<0) {
        err(1,"waiting error");
    }
    if(!WIFEXITED(status)) {
        err(2,"child was killed");
    }
    if(WEXITSTATUS(status)!=0) {
         err(3,"Error exit status differen than 0");
    }
    return status;
}


int wrapper_dup2(int old, int new) {
    int d = dup2(old,new);
    if(d<0) {
        err(5,"Error dup2");
    }
    return d;
}

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}
int wrapper_pipe(int pf[2]) {
    int l = pipe(pf);
    if(l<0) {
        err(7,"error piping");
    }
    return l;
}
//find ~ -printf "%T@ %p\n" | sort -nr | head -n 1 |cut -d ' ' -f2
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(1,"Incorrect arguments");
    }
    const int numberOfCommands = 4;
    const char* commands[][10] = {
        {"find",argv[1],"-printf","%T@ %p\n",NULL},
        {"sort","-nr",NULL},
        {"head","-n","1",NULL},
        {"cut","-d"," ","-f2",NULL},
        {NULL}
    };

    int prevFd;
    int i=0;
    for(;i<numberOfCommands-1;i++) {
        int pf[2];
        wrapper_pipe(pf);
        pid_t pid = wrapper_fork();
        if(pid==0) {
            if(i!=0) {
                wrapper_dup2(prevFd,0);
                close(prevFd);
            }
            close(pf[0]);
            wrapper_dup2(pf[1],1);
            close(pf[1]);
            
            execvp(commands[i][0],(char* const*)(void*)commands[i]);
            err(5,"Error execing");
        }
        close(pf[1]);
        if(i!=0) {
            close(prevFd);
        }
        prevFd = pf[0];
    }

    for(int j=0;j<numberOfCommands-1;j++) {
        wrapper_wait();
    }
    wrapper_dup2(prevFd,0);
    close(prevFd);
    execvp(commands[i][0],(char* const*)(void*)commands[i]);
    err(5,"Error execing");
    
}
