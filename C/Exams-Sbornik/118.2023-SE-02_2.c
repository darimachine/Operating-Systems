#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int wrapper_wait(void) {
    int status;
    if(wait(&status)<0) {
        err(1,"waiting error");
    }
    //if(!WIFEXITED(status)) {
      //  err(2,"child was killed");
    //}
    if(WEXITSTATUS(status)!=0) {
        err(3,"Error exit status differen than 0");
    }
    return status;
}

int wrapper_read(int fd, void * buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(3,"Error reading");
    }
    return r;
}
int wrapper_write(int fd, const void * buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(4,"Error writing");
    }
    return w;
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
int wrapper_lseek(int fd,off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(7,"error lseeking");
    }
    return l;
}
int pids[1024];
int pf[2];
void handleSingleProgram(const char* program) {
    
    wrapper_dup2(pf[1],1);
    close(pf[1]);
    execlp(program,program,(char*)NULL);
    err(5,"Error execing");
}
int main(int argc,char* argv[]) {
    if(argc<2) {
        errx(26,"There is no command to run");
    }
    int exitCode =1;
    if(pipe(pf) < 0) {
        err(5,"Error piping");
    }
    for(int i=1;i<argc;i++) {
        int pid = wrapper_fork();
        if(pid ==0) {
            close(pf[0]);
            handleSingleProgram(argv[i]);
        }
        pids[i-1] = pid;
    }
    close(pf[1]);
    char byte;
    const char* search = "found it!";
    size_t idx=0;
    while(wrapper_read(pf[0],&byte,sizeof(byte)) == sizeof(byte)) {
        if(byte == search[idx]) {
            idx++;
            if(idx==strlen(search)) {
                //must kill every process;
                for(int i=0;i<argc-1;i++) {
                    if(kill(pids[i],SIGTERM) < 0) {
                        err(26,"Error kiling");
                    }
                }
                exitCode=0;
                break;
            }
        }
        else {
            idx=0;
        }
    
    }
    close(pf[0]);
    
    for(int i=1;i<argc;i++) {
        wrapper_wait();
    }
    exit(exitCode);
}
