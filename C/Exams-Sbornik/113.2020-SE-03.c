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
int wrapper_lseek(int fd,off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(7,"error lseeking");
    }
    return l;
}
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) <0) {
        err(3,"error fstating");
    }
    return info.st_size;
    
}
typedef struct Triple{
    char filename[8];
    uint32_t offset;
    uint32_t length;
} Triple;
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(3,"incorrect arguments");
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(4,"Error opening file");
    }
    if(getFileSize(fd) % sizeof(Triple) !=0) {
        errx(4,"Incorrect file size");
    }
    Triple t;
    int size = getFileSize(fd) / sizeof(Triple);
    int pf[2];
    if(pipe(pf)<0) {
        err(5,"Error on pipe");
    }
    for(int i=0;i<size;i++) {
        wrapper_read(fd,&t,sizeof(t));
        pid_t pid = wrapper_fork();
        if(pid==0) {
            close(pf[0]);
            close(fd);
            int fd1=open(t.filename,O_RDONLY);
            if(fd1<0) {
                err(6,"Error on opening fd1");
            }
            if((long unsigned int)getFileSize(fd1) < (t.offset + t.length) * sizeof(uint16_t)) {
                errx(8,"Incorrect file size %s",t.filename);
            }
            wrapper_lseek(fd1,t.offset * sizeof(uint16_t),SEEK_SET);
            uint16_t result=0;
            uint16_t byte;
            for(uint32_t j=0;j<t.length;j++) {
                wrapper_read(fd1,&byte,sizeof(byte));
                result = result^byte;
            }
            wrapper_write(pf[1],&result,sizeof(result));
            close(fd1);
            close(pf[1]);
            exit(0);
        }

    }
    close(fd);
    close(pf[1]);
    uint16_t res=0;
    uint16_t byte;
    for(int i=0;i<size;i++) {
        wrapper_read(pf[0],&byte,sizeof(byte));
        res^=byte;
    }
    char buff[1024];
    int len =snprintf(buff,1024,"%X\n",res);
    wrapper_write(1,buff,len);

    for(int i=0;i<size;i++) {
        wrapper_wait();
    }

    close(pf[0]);
    

}
