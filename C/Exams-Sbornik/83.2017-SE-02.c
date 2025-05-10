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
        err(1,"Eror reading");
    }
    return r;
}

int wrapper_write(int fd, const void* buff, int size) {
    int r = write(fd,buff,size);
    if(r<0) {
        err(2,"Error writing");
    }
    return r;
}

uint32_t count=1;

void printData(int fd, bool isNgiven) {
    char ch;
    bool whenToPrint=true;
    while(wrapper_read(fd,&ch,sizeof(ch) == sizeof(ch))) {
        
        if (isNgiven == false) {
            wrapper_write(1,&ch,sizeof(ch));
        }
        else{
            
            if(whenToPrint==true) {
                char buff[1024];
                int len = snprintf(buff,sizeof(buff),"%d ",count);
                wrapper_write(1,buff,len);
                whenToPrint=false;
            }
            if(ch=='\n') {
                whenToPrint=true;
                count++;
            }
            wrapper_write(1,&ch,sizeof(ch));
        }
    }
}
int main(int argc, char* argv[]) {
    
    bool isNDefined=false;
    int start=1;
    if (argc==1) {
        printData(0,false);
    }
    else if(argc==2) {
        if(strcmp(argv[1],"-n")==0) {
            printData(0,true);
            start=2;
        }
    }
    else if(argc>2) {
        if(strcmp(argv[1],"-n")==0) {
            isNDefined=true;
            start=2;
        }
    }
    for(;start<argc;start++) {

        if(strcmp(argv[start],"-")==0) {
            printData(0,isNDefined);
        }
        else {
            int fd = open(argv[start],O_RDONLY);
            if(fd<0) {
                err(6,"Error opening file");
            }
            printData(fd,isNDefined);
            close(fd);
        }
    }
}
