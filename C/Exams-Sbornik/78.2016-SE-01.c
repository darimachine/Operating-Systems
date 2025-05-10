#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(3,"Error reading");
    }
    return r;
}
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(1,"error mismatch arguments");
    }
    int fd1=open(argv[1],O_RDONLY);
    if(fd1<0) {
        err(2,"Error opening file");
    }
    uint16_t arr[256]={0};
    unsigned char c;
    while(wrapper_read(fd1,&c,sizeof(c)) == sizeof(c)) {
        arr[c]++;
    }
    close(fd1);
    int fd = open(argv[1], O_WRONLY | O_TRUNC | O_CREAT,0666);
    if(fd<0) {
        err(4,"File couldn open");
    }
    for(int i=0;i<256;i++) {
        unsigned char curr=i;
        for(int j=0;j<arr[i];j++) {
            if (write(fd,&curr,sizeof(char)) < 0) {
                err(5,"Couldn write");
            }
        }
    }
    close(fd);
    
}
