#include <err.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>
typedef struct Trio{
    uint16_t offset;
    uint8_t original;
    uint8_t new;
} Trio;

int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(2,"Error reading");
    }
    return r;
}
int wrapper_write(int fd, const void* buff, int size) {
    int w=  write(fd,buff,size);
    if(w<0) {
        err(3,"Error writing");
    }
    return w;
}
void copyFiles(int src,int dest) {
    uint8_t ch;
    while(wrapper_read(src,&ch,sizeof(ch)) == sizeof(ch)) {
        wrapper_write(dest,&ch,sizeof(ch));
    }
}
int getFileSize(int fd) {
    struct stat info;
    if( fstat(fd,&info) <0 ) {
        err(6,"Error on fstat");
    }
    return info.st_size;
}
off_t wrapper_lseek(int fd, off_t offset,int whence) {
    off_t l = lseek(fd,offset,whence);
    if(l<0) {
        err(11,"Error lseeking");
    }
    return l;
}
int main(int argc, char* argv[]) {

    if(argc!=4) {
        errx(1,"Incorrect args");
    }
    int fd1= open(argv[2],O_RDONLY);
    if(fd1<0) { err(4,"Error opening fd1");}
    int fd2= open(argv[3],O_RDWR | O_CREAT | O_TRUNC, 0666);
    if(fd2<0) {
        err(5,"Error opening f2");
    }
    int patchFd=open(argv[1],O_RDONLY);
    if(patchFd<0) {
        err(8,"Error opening patchFD");
    }
    copyFiles(fd1,fd2);
    
    Trio t;
    if(getFileSize(patchFd) % sizeof(t) !=0 ){
        errx(9,"Incorrect format for patch file");
    }
    while(wrapper_read(patchFd,&t,sizeof(t))==sizeof(t)) {
        if(t.offset > getFileSize(fd1)) {
            errx(10,"Invalid offset");
        }
        wrapper_lseek(fd1,t.offset,SEEK_SET);
        wrapper_lseek(fd2,t.offset,SEEK_SET);
        uint8_t byte;
        wrapper_read(fd1,&byte,sizeof(byte));
        //printf("byte: %d  original: %d  new: %d\n",byte,t.original,t.new);
        if(byte != t.original) {
            errx(11,"Incorrect data in patch.bin on original");
        }
        wrapper_write(fd2,&t.new,sizeof(t.new));
    }

}
