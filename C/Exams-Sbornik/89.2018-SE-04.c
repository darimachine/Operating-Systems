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
        err(1,"Error reading");
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
size_t getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) == -1 ) {
        err(4,"Error fstating");
    }
    return info.st_size;
}

void mySort(int src,int dest) {
    size_t size1 = getFileSize(src)/sizeof(uint16_t);
    uint16_t ch1;
    uint16_t ch2;
    bool visited[65536] = {false};
    for(size_t i=0;i<size1;i++) {
        size_t minIndex=i;
        wrapper_lseek(src,0,SEEK_SET);
        wrapper_read(src,&ch1,sizeof(ch1));
        for(size_t j=1;j<size1;j++) {
            wrapper_read(src,&ch2,sizeof(ch2));
            if(visited[j] == true) {
                continue;
            }
            if(ch2<ch1) {
                minIndex=j;
                ch1=ch2;
            }
        }

        visited[minIndex]=true;
        wrapper_write(dest,&ch1,sizeof(ch1));
    }
}
int main(int argc, char* argv[]) {
    //uint16_t arr[9] = {9,7,8,4,6,1,5,3,2};
    //int m = open("proba",O_WRONLY | O_CREAT | O_TRUNC,0666);
    //wrapper_write(m,arr,sizeof(arr));
    if(argc!=3) {
        errx(2,"incorrect args");
    }
    int fd1 = open(argv[1],O_RDONLY);
    if(fd1<0) {
        err(3,"error opening argv1");
    }
    int fd2=open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd2<0) {
        err(4,"Error openign argv2");
    }
    mySort(fd1,fd2);
    close(fd1);
    close(fd2);
}
