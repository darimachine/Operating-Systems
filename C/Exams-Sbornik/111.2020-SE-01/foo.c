#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>

int wrapper_dup2(int old, int new) {
    int d = dup2(old,new);
    if(d<0) {
        err(5,"Error dup2");
    }
    return d;
}

int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(2,"Incorrect Number of Parameters");
    }
    if(mkfifo("temp.dat",0666) ==-1) {
        err(2,"Could create file");
    }
    int fd = open("temp.dat",O_WRONLY);
    if(fd<0) {
        err(2,"Error on open");
    }

    wrapper_dup2(fd,1);
    close(fd);
    execlp("cat","cat",argv[1],(char*)NULL);
    err(4,"Error on exec");
    

}
