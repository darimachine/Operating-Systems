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
    if(!WIFEXITED(status)) {
        err(2,"child was killed");
    }
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

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}

int pf[2][2];
const char* arr[] = {"DING ","DONG\n"};
void executeChild(void) {

    pid_t child = wrapper_fork();
    if(child==0) {
        close(pf[0][1]);
        close(pf[1][0]);
        uint8_t byte;
        while(wrapper_read(pf[0][0],&byte,sizeof(byte)) == sizeof(byte)) {
            wrapper_write(1,arr[1],strlen(arr[1]));
            uint8_t random=0;
            wrapper_write(pf[1][1],&random,1);
        }
        exit(0);
    }
}
int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(3,"Invalid argumnets");
    }
    int N = atoi(argv[1]);
    int D = atoi(argv[2]);
    if(N<1 || N>9) {
        errx(5,"N must be one digit");
    }
    if(D<1 || D>9) {
        errx(7,"D must be one digit");
    }

    for(int i=0;i<2;i++) {
        if(pipe(pf[i]) < 0) {
            err(2,"Error while piping");
        }
    }
    executeChild();
    close(pf[0][0]);
    close(pf[1][1]);

    for(int i=0;i<N;i++) {
        wrapper_write(1,arr[0],strlen(arr[0]));
        uint8_t signal=0;
        wrapper_write(pf[0][1],&signal,1);
        wrapper_read(pf[1][0],&signal,1);
        sleep(D);
    }

    close(pf[1][0]);
    close(pf[0][1]);
    wrapper_wait();
    
}
