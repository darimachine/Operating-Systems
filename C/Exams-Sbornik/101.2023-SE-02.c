#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>

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
int main(int argc, char* argv[]) {
    //0x00
    //duma ot 1 do 63
    //razdelitel nov red
    //opisanie na duma chetem do nulev bayt
    if(argc !=3) {
        errx(4,"Invalid arguments");
    }

    int fd = open(argv[2],O_RDONLY);
    if(fd<0) {
        err(5,"Error opening file");
    }
    int l = 0;
    int r = getFileSize(fd);
    bool isFound=false;
    while(l<=r) {
        int mid = l+(r-l)/2;
        
        wrapper_lseek(fd,mid,SEEK_SET);
        
        //do nov red
        char byte;
        while(wrapper_read(fd,&byte,sizeof(byte))) {
            if(byte==0x00) {
                break;
            }   
        }
        
        //chetem dumata
        char word[64];
        for(int i=0;i<64;i++) {
            wrapper_read(fd,&byte,sizeof(byte));
            if(byte== '\n' ) {
                word[i]='\0';
                break;
            }
            word[i]=byte;
        }
        
        printf("Duma: %s\n",word);
        
        if(strcmp(argv[1],word) == 0){
            printf("Vlizame li tuka\n");
            while(wrapper_read(fd,&byte,sizeof(byte))) {
                if(byte==0x00) {
                    break;
                }
                wrapper_write(1,&byte,sizeof(byte));
            }
            isFound=true;
            wrapper_write(1,"\n",1);
            break;
        }
        if(strcmp(argv[1],word) < 0) {
            r=mid-1;
        }
        else if(strcmp(argv[1],word) > 0) {
            l=mid+1;
        }
    }
    if(isFound==false) {
        char buffer[1024];
        int len = snprintf(buffer,sizeof(buffer),"The word %s is not found in the dictionary\n",argv[1]);
        wrapper_write(1,buffer,len);
    }
    close(fd);
}
