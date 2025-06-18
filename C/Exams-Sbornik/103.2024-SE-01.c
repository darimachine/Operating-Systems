#include <unistd.h>
#include <sys/stat.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h> // for testing ONLY!!
#include <fcntl.h>
#include <errno.h>

char template[] = "tempXXXXXX";

int wrapper_unlink(void) {
    int u = unlink(template);
    if(u<0) {
        err(7,"Error on unlink");
    }
    return u;
}
int wrapper_read(int fd,void* buff,int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        wrapper_unlink();
        err(1,"Error reading");
    }
    return r;
}

int wrapper_write(int fd, const void* buff,int size) {
    int w=write(fd,buff,size);
    if(w<0) {
        wrapper_unlink();
        err(2,"Error writing, %d",errno);
    }
    return w;
}

off_t wrapper_lseek(int fd, off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        wrapper_unlink();
        err(3,"Error lseeking");
    }
    return l;
}
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info)<0) {
        err(4,"Error on fstat");
    }
    return info.st_size;
}
typedef struct Node{
    uint64_t next; //0 ako sled vuzela nqma sledvasht
    char user_data[504];
} Node;
void copy(int dest, int source) {
    char byte;
    wrapper_lseek(source,0,SEEK_SET);
    wrapper_lseek(dest,0,SEEK_SET);
    while(wrapper_read(source,&byte,sizeof(byte))==sizeof(byte)) {
        wrapper_write(dest,&byte,sizeof(byte));
    }
    
}
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(5,"invalid number of arguments");
    }
    int fd = open(argv[1],O_RDWR);
    if(fd<0) {
        err(6,"Error opening this fd");
    }
    int size = getFileSize(fd);
    if(size%sizeof(Node)!=0) {
        errx(7,"file is corupted");
    }
    int fd2 = mkstemp(template); 
    if(fd2<0) {
        err(8,"error opening temp file md");
    }
    //filing with zeros
    wrapper_lseek(fd2,size-1,SEEK_SET);
    wrapper_write(fd2,"\0",1);
    wrapper_lseek(fd2,0,SEEK_SET);
    //end

    Node n;
    while(wrapper_read(fd,&n,sizeof(n)) == sizeof(n)) {
        if(n.next*sizeof(n) + sizeof(n) > (long unsigned int)size) {
            errx(9,"Invalid next number out of range");
        }
        
        wrapper_write(fd2,&n,sizeof(n));
        wrapper_lseek(fd2,n.next*sizeof(n),SEEK_SET);
        wrapper_lseek(fd,n.next *sizeof(n),SEEK_SET);
        
        if(n.next==0){
            break;
        }

    }
    copy(fd,fd2);
    close(fd);
    wrapper_unlink();
}
