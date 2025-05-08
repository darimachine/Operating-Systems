//Реализирайте команда cat, работеща с произволен брой подадени входни параметри.
#include <err.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include<ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <err.h>


int wrapped_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0){
        err(1,"read error");
    }
    return r;
}
int main(int argc, char* argv[]) {
    if(argc<2) {
        errx(1,"You must have at least 2 args");
    }
    for(int i=1;i<argc;i++) {
        int fd1= open(argv[i],O_RDONLY);
        if(fd1<0) {
            err(2,"Error opening");
        }
        char buffer[4096];
        int readed;
        while((readed = wrapped_read(fd1,buffer,sizeof(buffer))) > 0) {
            if(write(1,buffer,readed) < 0) {
                err(3,"Error writing"); 
            }
        }
        close(fd1);
    }
}
