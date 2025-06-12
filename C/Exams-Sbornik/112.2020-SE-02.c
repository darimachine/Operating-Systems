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
    if(wait(&status) < 0) {
        err(1,"Error waiting");
    }
    if(!WIFEXITED(status)){
        errx(2,"Error IfExited");
    }
    if(WEXITSTATUS(status)!=0) {
        errx(3,"Error exit status differ than 0");
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

int main(int argc, char* argv[]) {
    
    if(argc!=3) {
        errx(3,"Error invalid args");
    }
    int pf[2];
    if(pipe(pf) < 0) {
        err(3,"Error piping");
    }
    pid_t pid = wrapper_fork();
    if(pid==0) {
        close(pf[0]);
        wrapper_dup2(pf[1],1);
        close(pf[1]);
        execlp("cat","cat",argv[1],(char*)NULL);
        err(5,"Error execing");
    }
    close(pf[1]);
    int fd = open(argv[2],O_CREAT | O_TRUNC | O_WRONLY , 0666);
    if(fd<0) {
        err(6,"Error opening");
    }

    uint8_t byte;
    while(wrapper_read(pf[0],&byte,sizeof(byte)) == sizeof(byte)) {
        
        if(byte == 0x7D) {
            
            wrapper_read(pf[0],&byte,sizeof(byte));
            byte = byte ^0x20;
            if(byte == 0x00 || byte == 0xFF || byte == 0x55 || byte == 0x7D) {
                wrapper_write(fd,&byte,sizeof(byte));
            }
            else{
                errx(4,"Incorrect file format");
            }
        }
        else if(byte != 0x55) {
            wrapper_write(fd,&byte,sizeof(byte));
        }

    }

    close(pf[0]);
    close(fd);
    wrapper_wait();

}
