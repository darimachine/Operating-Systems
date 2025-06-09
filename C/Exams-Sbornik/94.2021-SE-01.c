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
    uint8_t byte;
    while(wrapper_read(fd,&byte,sizeof(byte)) == sizeof(byte)) {
            uint16_t encode=0x00;
            for(int i=0;i<8;i++) {
                if( (byte & 1<<i) ==0 ) {
                    encode |= 1 << (i*2);
                }
                else{
                    encode |= 2 << (i*2);
                }
            }
            uint8_t* encodedByte = (uint8_t*)&encode; // reinterpret_cast
            wrapper_write(fd1,encodedByte +1,sizeof(uint8_t));
            wrapper_write(fd1,encodedByte,sizeof(uint8_t));
            // TESTING
            char buff[1024];
            snprintf(buff,1024,"%x\n",encode);
            wrapper_write(1,buff,strlen(buff));
    }
    close(fd);
    close(fd1);

}
