#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>

pid_t wrapper_fork(void) {
    pid_t pid = fork();
    if(pid<0) {
        err(3,"Error on fork");
    }
    return pid;
}   

int wrapper_dup2(int old,int new) {
    int d = dup2(old,new);
    if(d<0) {
        err(2,"Error on dup2");
    }
    return d;
}
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(12,"Error reading");
    }
    return r;
}
int wrapper_write(int fd, const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(13,"Error writing");
    }
    return w;
}
int exitCode=0;

int main(int argc, char* argv[]) {
    if(argc!=4){
        errx(6,"Invalid arguments");
    }
    int N = atoi(argv[2]);
    int nullFd = open("/dev/null",O_WRONLY);
    int randomFd=open("/dev/urandom",O_RDONLY);
    int outFd=open(argv[3],O_WRONLY | O_CREAT | O_TRUNC,0666);
    //char buffer[UINT16_MAX];
    for(int i=0;i<N;i++) {
        
        int pf[2];
        if(pipe(pf) < 0) {
            err(11,"error piping");
        }
        char buffer[UINT16_MAX];
        uint16_t S;
        wrapper_read(randomFd,&S,sizeof(S));
        wrapper_read(randomFd,buffer,S); 
        wrapper_write(pf[1],buffer,S);
        
        pid_t pid=wrapper_fork();
        if(pid==0) {
            
            close(pf[1]);
            wrapper_dup2(nullFd,1);
            wrapper_dup2(nullFd,2);
            wrapper_dup2(pf[0],0);
            close(pf[0]);
            close(randomFd);
            close(nullFd);
            close(outFd);

            execlp(argv[1],argv[1],(char*)NULL);
            err(12,"Error on exec");

        }
        close(pf[0]);
        close(pf[1]);
        int status;
        if(wait(&status)<0){
            err(5,"error on waiting");
        }
        if(WIFSIGNALED(status)) {
            
            wrapper_write(outFd,buffer,S);
            close(nullFd);
            close(randomFd);
            close(outFd);
            exit(42);
            break;
        }
        
    }

    close(nullFd);
    close(randomFd);
    close(outFd);
    exit(0);

}
