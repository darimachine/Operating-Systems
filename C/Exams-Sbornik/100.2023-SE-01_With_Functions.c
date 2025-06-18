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
int isMessageValid(int fd) {
    uint8_t calculate=0;
    uint8_t N;
    wrapper_read(fd,&N,sizeof(N));
    calculate^=0x55;
    calculate^=N;
    uint8_t data;
    for(uint8_t i=3;i<N;i++) {
        wrapper_read(fd,&data,sizeof(data));
        calculate^=data;
    }
    uint8_t checksum;
    wrapper_read(fd,&checksum,sizeof(checksum));
    if(checksum==calculate) {
        return N;
    }
    return -1;

}
void writeMessage(int from, int to, int N) {
        for(int i=0;i<N;i++) {
            uint8_t byte;
            wrapper_read(from,&byte,sizeof(byte));
            wrapper_write(to,&byte,sizeof(byte));
        }
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
            int N;
            if((N=isMessageValid(inFd)) !=-1) {
               wrapper_lseek(inFd,currPos-1,SEEK_SET);
               writeMessage(inFd,outFd,N);
            }
            else {
                wrapper_lseek(inFd,currPos,SEEK_SET);
            }

        }
    }
    close(inFd);
    close(outFd);

}
