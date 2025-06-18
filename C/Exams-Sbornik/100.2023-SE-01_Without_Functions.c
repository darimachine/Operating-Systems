#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

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

int wrapper_lseek(int fd,off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(7,"error lseeking");
    }
    return l;
}
int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(3,"Invalid arguments");
    }
    int inFd= open(argv[1],O_RDONLY);
    if(inFd<0) {
        err(5,"Error opening %s",argv[1]);
    }
    int outFd = open(argv[2],O_CREAT | O_TRUNC | O_WRONLY,0666);
    if(outFd<0) {
        err(6,"Error opening ");
    }
    uint8_t begin;
    int currPos;
    while(wrapper_read(inFd,&begin,sizeof(begin)) == sizeof(begin)) {
        if(begin == 0x55) {
            currPos = wrapper_lseek(inFd,0,SEEK_CUR);
            uint8_t calculate=0;
            uint8_t N;
            wrapper_read(inFd,&N,sizeof(N));
            calculate^=begin;
            calculate^=N;
            uint8_t buffer[4096];
            uint8_t data;
            for(uint8_t i=3;i<N;i++) {
                wrapper_read(inFd,&data,sizeof(data));
                calculate^=data;
                buffer[i-3] = data;
            }
            uint8_t checksum;
            wrapper_read(inFd,&checksum,sizeof(checksum));
            if(checksum == calculate) {
                wrapper_write(outFd,&begin,sizeof(begin));
                wrapper_write(outFd,&N,sizeof(N));
                wrapper_write(outFd,buffer,N-3);
                wrapper_write(outFd,&checksum,sizeof(checksum));
            }
            else {
                wrapper_lseek(inFd,currPos,SEEK_SET);
            }

        }
    }
    close(inFd);
    close(outFd);

}
