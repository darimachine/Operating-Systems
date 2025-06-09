#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

int wrapper_wait(void) {
    int status;
    if(wait(&status)) {
        err(1,"waiting error");
    }
    if(!WIFEXITED(status)) {
        err(2,"child was killed");
    }
    if(WEXITSTATUS(status)!=0) {
        err(3,"Error exit status differen than 0");
    }
    return status;
}

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
int wrapper_dup2(int old, int new) {
    int d = dup2(old,new);
    if(d<0) {
        err(5,"Error dup2");
    }
    return d;
}

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
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
    uint32_t magic; // 0xEFBEADDE
    uint8_t headerV;// 0x01
    uint8_t dataV; // 0x00 ИЛИ 0x01
    uint16_t count;
    uint32_t reserved1;
    uint32_t reserved2;
} Header;
typedef struct Version00{
    uint16_t offset;
    uint8_t originalByte;
    uint8_t newByte;
} Version00;
typedef struct Version01{
    uint32_t offset;
    uint16_t originalWord;
    uint16_t newWord;

} Version01;

void copyFile(int from,int to) {
    uint8_t byte;
    while(wrapper_read(from,&byte,sizeof(byte)) == sizeof(byte)) {
        wrapper_write(to,&byte,sizeof(byte));
    }
    wrapper_lseek(from,0,SEEK_SET);
    wrapper_lseek(to,0,SEEK_SET);
    
}
int main(int argc, char* argv[]) {
    if(argc!=4) {
        errx(4,"Incorrect argumnets");
    }
    int fd1 = open(argv[1],O_RDONLY);
    int fd2= open(argv[2],O_RDONLY);
    int fd3= open(argv[3],O_WRONLY | O_TRUNC | O_CREAT, 0666);
    if(fd1<0) {
        err(3,"Error opening %s",argv[1]);
    }
    if(fd2<0) {
        err(4,"Error opening %s", argv[2]);
    }
    if(fd3<0) {
        err(5,"Error opening %s",argv[3]);
    }
    
    Header h;
    wrapper_read(fd1,&h,sizeof(h));
    if(h.magic!=0xEFBEADDE) {
        errx(6,"Error magic number mismatch");
    }
    if(h.headerV!=0x01) {
        errx(8,"Error header version mismatch must be 0x01");
    }
    copyFile(fd2,fd3);
    if(h.dataV==0x00) {
        Version00 v;
        uint16_t count = h.count;
        for(int i=0;i<count;i++) {
            wrapper_read(fd1,&v,sizeof(v));
            wrapper_lseek(fd2,v.offset*sizeof(uint8_t),SEEK_SET);
            uint8_t byte;
            wrapper_read(fd2,&byte,sizeof(byte));
            if(v.originalByte == byte) {
                wrapper_lseek(fd3,v.offset*sizeof(uint8_t),SEEK_SET);
                wrapper_write(fd3,&v.newByte,sizeof(v.newByte));
            }
            else {
                errx(12,"File f1 is corrupted the byte doesnt match the original");
            }

        }

    }
    else if(h.dataV==0x01) {
        Version01 v;
        uint16_t count = h.count;
        for(int i=0;i<count;i++) {
            wrapper_read(fd1,&v,sizeof(v));
            wrapper_lseek(fd2,v.offset*sizeof(uint16_t),SEEK_SET);
            uint16_t byte;
            wrapper_read(fd2,&byte,sizeof(byte));
            if(v.originalWord == byte) {
                wrapper_lseek(fd3,v.offset*sizeof(uint16_t),SEEK_SET);
                wrapper_write(fd3,&v.newWord,sizeof(v.newWord));
            }
            else{
                errx(12,"File f1 is corrupted the byte doesnt match the original");
            }
        }
    }
    else {
        errx(9,"Error data version not found");
    }
    close(fd1);
    close(fd2);
    close(fd3);
    
}
