#include <err.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>

int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(1,"Eror reading");
    }
    return r;
}

int wrapper_write(int fd, const void* buff, int size) {
    int r = write(fd,buff,size);
    if(r<0) {
        err(2,"Error writing");
    }
    return r;
}
off_t wrapper_lseek(int fd, off_t offset,int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(3,"Error lseeking");
    }
    return l;
}
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) == -1 ) {
        err(4,"Error fstating");
    }
    return info.st_size;
}

void selectionSort(int src) {
    int size1 = getFileSize(src)/sizeof(uint32_t);
    uint32_t ch1;
    uint32_t ch2;
    
    for(int i=0;i<size1-1;i++) {
        
        size_t iNumberIndex=i*sizeof(uint32_t);
        size_t minIndex=iNumberIndex;

        wrapper_lseek(src,iNumberIndex,SEEK_SET);
        wrapper_read(src,&ch1,sizeof(ch1));
        for(int j=i+1;j<size1;j++) {
            wrapper_read(src,&ch2,sizeof(ch2));
            if(ch2<ch1) {
                minIndex= j * sizeof(uint32_t);
                ch1=ch2;
            }

        }
        if(minIndex!=iNumberIndex) {
            wrapper_lseek(src,iNumberIndex,SEEK_SET);
            uint32_t curr;
            wrapper_read(src,&curr,sizeof(curr));
            wrapper_lseek(src,minIndex,SEEK_SET);
            wrapper_write(src,&curr,sizeof(curr));
            wrapper_lseek(src,iNumberIndex,SEEK_SET);
            wrapper_write(src,&ch1,sizeof(ch1));
        }
    }
}
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(2,"incorrect args");
    }
    int fd1 = open(argv[1],O_RDWR);
    if(fd1<0) {
        err(3,"error opening argv1");
    }
    
    selectionSort(fd1);
    close(fd1);
    
}
