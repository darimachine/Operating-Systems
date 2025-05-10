#include <stdint.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct pair {
    uint32_t x;
    uint32_t y;
} pair;

uint32_t getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) < 0) {
        err(2,"Error stating");
    }
    return info.st_size;
}
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(7,"Error reading file");
    }
    return r;
}
off_t wrapper_lseek(int fd, off_t offset, int whence) {
    int off = lseek(fd,offset,whence);
    if(off < 0) {
        err(10,"Error lseekiing");
    }
    return off;
}
int main(void) {
    int fd1=open("f1.bin",O_RDONLY);
    if(fd1<0) {
        err(1,"Error opening file");
    }
    int fd2=open("f2.bin",O_RDONLY);
    if(fd2<0) {
        err(3,"Error opening file f2");
    }
    if(getFileSize(fd1) % sizeof(pair) !=0) {
        errx(4,"Inccorect file format");
    }
    if (getFileSize(fd2) % sizeof(uint32_t) !=0 ) {
        errx(5,"Incorect file format for f2.bin");
    }

    int fd3= open("f3.bin",O_WRONLY | O_CREAT | O_TRUNC,0666);
    
    if(fd3<0) {
        err(6,"Error opening f3");
    }
    
    pair p;
    
    while(wrapper_read(fd1,&p,sizeof(p))==sizeof(p)) {
        
        if(p.x + p.y > getFileSize(fd2)/sizeof(uint32_t)) {
            errx(8,"end of file reached incorrect offset");
        }
        wrapper_lseek(fd2,p.x*sizeof(uint32_t),SEEK_SET);
        
        uint32_t num;

        for(uint32_t i=0;i<p.y;i++) {
            wrapper_read(fd2,&num,sizeof(num));
            if (write(fd3,&num,sizeof(num)) < 0) {
                err(9,"Error writing num %d",num);
            }
        }
        
    }

    close(fd1);
    close(fd2);
    close(fd3);
}
