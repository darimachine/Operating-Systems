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
typedef struct Header{

    uint16_t magic;
    uint16_t filetype;
    uint32_t count;
} Header;
int main(int argc, char* argv[]) {

    if(argc!=4) {
        errx(3,"Invalid number of arguments");
    }
    int list = open(argv[1],O_RDONLY);
    if(list <0) {
        err(4,"Invalid open");
    }
    int data = open(argv[2],O_RDONLY);
    if(data<0) {
        err(4,"Invalid open");
    }
    int out = open(argv[3], O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(out<0) {
        err(3,"Invalid open");
    }
    int listSize = getFileSize(list);
    if((listSize - sizeof(Header)) % sizeof(uint16_t) !=0){
        errx(4,"Invalid list.bin fileSize");
    
    }
    Header hList;
    
    wrapper_read(list,&hList,sizeof(hList));
    if((listSize - sizeof(Header)) / sizeof(uint16_t) !=hList.count || hList.filetype!=1) {
        errx(6,"invalid count in the list file or type");
    }
    int dataSize=getFileSize(data);
    Header dList;
    wrapper_read(data,&dList,sizeof(dList));
    
    if((dataSize - sizeof(Header))% sizeof(uint32_t) !=0 || dList.filetype !=2){
        errx(4,"Invalid data.bin fileSize or type");
    }
    if((dataSize-sizeof(Header)) /sizeof(uint32_t) !=dList.count) {
        errx(5,"Invalid count in the data file");
    }

    Header toSave;
    toSave.magic = 0x5A4D;
    toSave.filetype=3;
    
    int index=0;// otmestvane v data.bim
    uint16_t listByte; // otmestvane v out.bin
    uint32_t maxCount=0;
    for(uint16_t i=0;i<hList.count;i++) {
        wrapper_read(list,&listByte,sizeof(listByte));    
        wrapper_lseek(data,index * sizeof(uint32_t)  + sizeof(Header),SEEK_SET);
        uint32_t toRead;
        wrapper_read(data,&toRead,sizeof(toRead));
        index++;
        if(maxCount < listByte) {
            maxCount = listByte;
        }
        if(toRead==0) {continue;}
        uint64_t toWrite=toRead;
        wrapper_lseek(out,listByte * sizeof(uint64_t) + sizeof(Header),SEEK_SET);
        wrapper_write(out,&toWrite,sizeof(toWrite));
        
    }
    wrapper_lseek(out,0,SEEK_SET);
    toSave.count=maxCount+1;
    wrapper_write(out,&toSave,sizeof(toSave));
    close(list);
    close(data);
    close(out);

}
