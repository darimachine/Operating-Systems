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
typedef struct DataHeader{
    uint32_t magic;
    uint32_t count;
} DataHeader; // danni uint64_t chisla
typedef struct CompHeader{
    uint32_t magic1;
    uint16_t magic2;
    uint16_t reserver;
    uint64_t count;
} CompHeader;
typedef struct CompData {
    uint16_t type; // 0 po malko, 1 po golqmo
    uint16_t reserved[3];
    uint32_t offset1; // otmestvane broy elementi na data.bin offset1*uint64_t + header
    uint32_t offset2;
} CompData;

void swap(int fd, uint32_t offset1, uint32_t offset2) {
    //READ
    uint64_t element1;
    wrapper_lseek(fd,offset1*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
    wrapper_read(fd,&element1,sizeof(element1));
    uint64_t element2;
    wrapper_lseek(fd,offset2*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
    wrapper_read(fd,&element2,sizeof(element2));
    //SWAP
    wrapper_lseek(fd,offset1*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
    wrapper_write(fd,&element2,sizeof(element2));

    wrapper_lseek(fd,offset2*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
    wrapper_write(fd,&element1,sizeof(element1)); 
}
int main(int argc, char* argv[]) {
    if(argc!=3) {
        err(2,"Invalid number of arguments");
    }
    int dataFd = open(argv[1],O_RDWR);
    if(dataFd<0) {
        err(5,"Error opening dataFd");
    }
    int compFd= open(argv[2],O_RDONLY);
    if(compFd<0) {
        err(6,"Error opening compFd");
    }
    DataHeader dataHeader;
    wrapper_read(dataFd,&dataHeader,sizeof(dataHeader));
    if(dataHeader.magic != 0x21796F4A) {
        errx(3,"Invalid data header magic element");
    }
    int dataSize = getFileSize(dataFd);
    if((dataSize - sizeof(DataHeader)) % sizeof(uint64_t) !=0) {
        errx(4,"Invalid file size in the data section");
    }
    CompHeader compHeader;
    wrapper_read(compFd,&compHeader,sizeof(compHeader));
    if(compHeader.magic1!=0xAFBC7A37 || compHeader.magic2!=0x1C27) {
        errx(5,"Invalid magic number in the comp file");
    }
    int compSize = getFileSize(compFd);
    if((compSize-sizeof(CompHeader))% sizeof(CompData)!=0) {
        errx(6,"Invalid data size in the comp file");
    }
    CompData data;
    while(wrapper_read(compFd,&data,sizeof(data)) == sizeof(data)) {

        uint64_t element1;
        wrapper_lseek(dataFd,data.offset1*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
        wrapper_read(dataFd,&element1,sizeof(element1));
        uint64_t element2;
        wrapper_lseek(dataFd,data.offset2*sizeof(uint64_t) + sizeof(DataHeader),SEEK_SET);
        wrapper_read(dataFd,&element2,sizeof(element2));
        
        if(data.type==0) {
            if(element1>element2) {
                swap(dataFd,data.offset1,data.offset2);
            }
        }
        else if(data.type==1) {
            if(element1<element2) {
                swap(dataFd,data.offset1,data.offset2);
            }
        }
        else {
            errx(7,"Invalid type");
        }
    }

    close(dataFd);
    close(compFd);

}
