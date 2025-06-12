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
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) <0) {
        err(3,"error fstating");
    }
    return info.st_size;
    
}
int main(void) {
    int fd1= open("input.scl",O_RDONLY);
    if(fd1<0) {
        err(5,"Error opening");
    }
    int fd2= open("input.sdl",O_RDONLY);
    if(fd2<0) {
        err(5,"Error opening");
    }
    int fd3= open("out.bin",O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(fd3<0) {
        err(6,"Error opening");
    }
    //printf("%d\n",getFileSize(fd2));
    if(getFileSize(fd2) % sizeof(uint16_t) !=0) {
        errx(4,"Incorrect file format");
    }
    int file1Bits = getFileSize(fd1) * 8;
    int file2SizeInByte = getFileSize(fd2) / sizeof(uint16_t); 
    if(file1Bits != file2SizeInByte) {
        errx(6,"Incorrect file format for scl or sdl");
    }
    uint8_t sclByte;
    while(wrapper_read(fd1,&sclByte,sizeof(sclByte)) == sizeof(sclByte)) {
        uint16_t sdlByte;
        for(int i=0;i<8;i++) {
            wrapper_read(fd2,&sdlByte,sizeof(sdlByte));
            if((sclByte & 1<<(7-i)) !=0) {
                wrapper_write(fd3,&sdlByte,sizeof(sdlByte));
            }
        }
    }
    
    close(fd1);
    close(fd2);
    close(fd3);

}
