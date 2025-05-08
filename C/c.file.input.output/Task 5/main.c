// Напишете програма, която приема точно 2 аргумента. Първият може да бъде само --min, --max или --print (вижте man 3 strcmp). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа (uint16_t - вижте man stdint.h). Ако първият аргумент е:

--min - програмата отпечатва кое е най-малкото число в двоичния файл.
--max - програмата отпечатва кое е най-голямото число в двоичния файл.
--print - програмата отпечатва на нов ред всяко число.
Използвайте двоичния файл binary/dump или генерирайте сами такъв (правилно подравнен).
#include <err.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include<ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
int wrapper_write(int fd, const void* buff,int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(2,"Error writing");
    }
    return w;
}
int wrapper_read(int fd, void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(3,"Error Reading");
    }
    return r;
}
bool isValidArgs(const char * arg) {
    if(strcmp(arg,"--min")==0 || strcmp(arg,"--max")==0 || strcmp(arg, "--print")==0) {
        return true;
    }
    return false;
}

void printnumToText(uint16_t num) {
    char buff[24];
    int len = snprintf(buff,sizeof(buff),"%d\n",num);
    wrapper_write(1,buff,len);
}

void printNumbers(int fd) {
    uint16_t num;

    while(wrapper_read(fd,&num,sizeof(num))>0) {
        printnumToText(num);
    }
}
uint16_t findMinMax(int fd, bool isMax) {
    uint16_t num;
    uint16_t res;
    
    if(wrapper_read(fd,&res,sizeof(res)) == 0) { // reading the first 1 and setting it to res
        errx(5,"Empty File");
    }

    while(wrapper_read(fd,&num,sizeof(num))>0) {
        if(isMax) {
            res = MAX(res,num);
        }
        else{
            res = MIN(res,num);
        }

    }
    return res;
}

int main(int argc, char * argv[]) {
    if (argc!=3) {
        errx(2,"args mismatch");
    }
    if(!isValidArgs(argv[1])) {
        errx(4, "Invalid args parameter");
    }
    int fd= open(argv[2],O_RDONLY);
    if(fd<0) {
        err(5,"error opening");
    }

    if (strcmp(argv[1],"--print")==0) {
        printNumbers(fd);
    }
    else if(strcmp(argv[1], "--max") == 0) {
        uint16_t res = findMinMax(fd,true);
        printnumToText(res);
    }
    else {
        uint16_t res = findMinMax(fd,false);
        printnumToText(res);
    }
    close(fd);

}   
