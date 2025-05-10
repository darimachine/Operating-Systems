#include <stdint.h>
#include <err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct Triple{
    uint16_t offset;
    uint8_t original;
    uint8_t new;
} Triple;

uint16_t getFileSize(int fd) {
    struct stat info;
    
    if(fstat(fd,&info) == -1) {
        err(2,"Error on fstat");
    }
    return info.st_size;
}
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(3,"Error reading");
    }
    return r;
}
int wrapper_write(int fd, const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(4,"Error writing");
    }
    return w;
}
int main(int argc, char* argv[]) {
    if(argc!=4) {
        errx(1,"Error incorrect arguments");
    }
    int fd1= open(argv[1],O_RDONLY);
    if(fd1<0) {
        err(5,"Error opening first file");
    }
    int fd2=open(argv[2],O_RDONLY);
    if(fd2<0) {
        err(6,"Error opening second file");
    }
    if(getFileSize(fd1) != getFileSize(fd2)) {
        errx(7,"Files differ by size");
    }
    int fd3=open(argv[3],O_WRONLY|O_CREAT|O_TRUNC,0666);
    if(fd3<0) {
        err(8,"Error opening file 3");
    }
    Triple t;
    for(uint16_t i=0;i<getFileSize(fd1);i++) {
        
        wrapper_read(fd1,&t.original,sizeof(t.original));
        wrapper_read(fd2,&t.new,sizeof(t.new));
        
        if(t.original != t.new) {
            t.offset=i;
            wrapper_write(fd3,&t,sizeof(t));
        }
    }

}
