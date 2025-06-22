#include <err.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int wrapper_write(int fd, const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(1,"Error writing");
    }
    return w;
}
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(2,"Error reading");
    }
    return r;

}
uint64_t getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info)<0) {
        err(4,"Error on fstat");
    }
    return info.st_size;

}

typedef struct Header {
    uint32_t magic;
    uint32_t packet_count;
    uint64_t original_size;
} Header;

int main(int argc, char * argv[]) {
    if(argc!=3) {
        errx(5,"Invalid args");
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(19,"Error opening");
    }
    int outFd=open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(outFd<0) {
        err(18,"Eror opening second arg");
    }
    Header h;
    wrapper_read(fd,&h,sizeof(h));
    if(h.magic !=0x21494D46) {
        errx(12,"Invalid magic number in the file");
    }
    char firstByte;
    for(uint32_t i=0;i<h.packet_count;i++) {
        wrapper_read(fd,&firstByte,sizeof(firstByte));
        uint8_t N = firstByte & (~(1<<7));
        char nextByte;
        if(firstByte & 1<<7) {
            wrapper_read(fd,&nextByte,sizeof(nextByte));
            for(int j=0;j<=N;j++) {
                wrapper_write(outFd,&nextByte,sizeof(nextByte));
            }
        }
        else {
            for(int j=0;j<=N;j++) {
                wrapper_read(fd,&nextByte,sizeof(nextByte));
                wrapper_write(outFd,&nextByte,sizeof(nextByte));
            }
        }
    }
    if(h.original_size != getFileSize(outFd)) {
        errx(32,"original size must match original file Size!");
    }
    close(fd);
    close(outFd);
}
