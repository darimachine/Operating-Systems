#include <err.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>

int wrapper_read(int fd, void* buff, int size) {
    int r =read(fd,buff,size);
    if(r<0) {
        err(1,"Error reading");
    }
    return r;
}
int wrapper_write(int fd, const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(2,"Error writing");
    }
    return w;
}
int findIndex(const char* str,char ch) {
    for(size_t i=0;i<strlen(str);i++) {
        if(str[i] ==ch) {
            return i;
        }
    }
    return -1;
}
void dOption(char* str) {
    char ch;
    while(wrapper_read(0,&ch,sizeof(ch))==sizeof(ch)) {
        if(findIndex(str,ch) == -1) {
            wrapper_write(1,&ch,sizeof(ch));
        }
    }
}

void sOption(char* str) {
   char prev;
   char ch;
   wrapper_read(0,&prev,sizeof(prev));
   wrapper_write(1,&prev,sizeof(prev));
   while(wrapper_read(0,&ch,sizeof(ch)) == sizeof(ch)) {
        if(ch==prev) {
            if(findIndex(str,ch)!=-1) { // ako se sudurja
                continue;
            }
        }
        wrapper_write(1,&ch,sizeof(ch));
        prev=ch;
   }
}

void replace(char* str1, char * str2) {
    char ch;
    while(wrapper_read(0,&ch,sizeof(ch)) == sizeof(ch)) {
        int index= findIndex(str1,ch);
        if(index == -1) {
            wrapper_write(1,&ch,sizeof(ch));
        }
        else{
            wrapper_write(1,&str2[index],sizeof(char));
        }
    }

}
int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(1,"Invalid args");
    }
    if(strcmp(argv[1],"-d")==0) {
        dOption(argv[2]);
    }
    else if(strcmp(argv[1],"-s")==0) {
        sOption(argv[2]);
    }
    else{
        if(strlen(argv[1]) != strlen(argv[2])) {
            errx(3,"Error lengthe of 2 string differ");
        }
        replace(argv[1],argv[2]);
    }
}
