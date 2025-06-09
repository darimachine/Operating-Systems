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
int wrapper_pipe(int fd[2]) {
    int p = pipe(fd);
    if(p < 0) {
        err(8,"error piping");
    }
    return p;
}
//cat /etc/passwd | cut -d : -f7 | sort | uniq -c| sort
int main(void) {
    
    const int numberOfCommands=5;
    const char* commands[][64] = { // 64 for the arguments count 
       {"cat","/etc/passwd",NULL}, 
        {"cut","-d",":","-f7",NULL},
        {"sort",NULL},
        {"uniq","-c",NULL},
        {"sort",NULL},
        {NULL}
    };
    
    int prevFd;
    int i=0;
    for(;i<numberOfCommands-1;i++) { // number of pipes
        int pf[2]; // pipe
        wrapper_pipe(pf);
        pid_t pid = wrapper_fork();
        if(pid==0) {
            if(i!=0) {
                wrapper_dup2(prevFd,0);
                close(prevFd);
            }
            wrapper_dup2(pf[1],1);
            close(pf[1]);
            close(pf[0]);
            
            execvp(commands[i][0],(char* const*)(void*)commands[i]);
            err(7,"Error on execvp");
        }
        close(pf[1]);
        if(i!=0) {
            close(prevFd);
        }
        prevFd=pf[0];

    }
    wrapper_dup2(prevFd,0);
    close(prevFd);
    for(int j=0;j<numberOfCommands-1;j++) {
        wrapper_wait();
    }
    execvp(commands[i][0],(char* const*)(void*)commands[i]);

}




