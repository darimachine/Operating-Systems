#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

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
int main(int argc, char* argv[]) {
    if (argc!=2) {
        errx(4,"Incorrect arguments");

    }
    int fd = open("temp.dat",O_RDONLY);
    if(fd<0) {
        err(2,"Error on opening");
    }
    wrapper_dup2(fd,0);
    close(fd);
    if(unlink("temp.dat") < 0) {
        err(3,"Error on unlink");
    }
    execlp(argv[1],argv[1],(char*)NULL);
    err(5,"Error execing");


}
