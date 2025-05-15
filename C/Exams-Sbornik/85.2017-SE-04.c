#include <err.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>


int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(1,"eror reading");
    }
    return r;
}
int wrapper_write(int fd, const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(2,"error writing");
    }
    return w;
}
void printData(int fd) {
    char ch;
    while(wrapper_read(fd,&ch,sizeof(ch)) == sizeof(ch)) {
        wrapper_write(1,&ch,sizeof(ch));
    }
}
int main(int argc, char * argv[]) {
    if (argc==1) {
        printData(0);
    }
    else {
        for(int i=1;i<argc;i++) {
            if(strcmp(argv[i],"-")==0) {
                printData(0);
            }
            else {
                int fd=open(argv[i],O_RDONLY);
                if(fd<0) {
                    err(4,"Error opening file");
                }
                printData(fd);
                close(fd);
            }
        }
    }

}
