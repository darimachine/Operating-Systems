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

int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(3,"Argument mismatch error");
    }

    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(5,"Error opening file %s",argv[1]);
    }
    int fd1= open(argv[2],O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(fd1<0) {
        close(fd1);
    }
    uint16_t byte;
    while(wrapper_read(fd,&byte,sizeof(byte)) == sizeof(byte)) {
            uint8_t decode=0x00;
            uint16_t mask1 = byte >> 8;
            uint16_t mask2=byte<<8;
            for(int i=0;i<8;i++) {
                if(i<4) {
                    if( (mask1 & 1<<((i*2))) ==0 ) {
                        decode |= 1 << i;
                    }
                }
                else {
                    if( (mask2 & 1<<((i*2))) ==0 ) {
                        decode |= 1 << i;
                    }

                }
            }
            
            wrapper_write(fd1,&decode,sizeof(decode));
           // TESTING!!!! 
            char buff[1024];
            snprintf(buff,1024,"%x\n",decode);
            wrapper_write(1,buff,strlen(buff));
    }
    close(fd);
    close(fd1);

}
