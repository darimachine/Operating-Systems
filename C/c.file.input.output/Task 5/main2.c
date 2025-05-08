// Za testovi celi
#include <err.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include<ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>


int main(void) {

    uint16_t arr[10] = {53,12,32,13,84,11,98,1,4,5};
    int fd=open("binary.bin",O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd,arr,sizeof(arr));
    //for(int i=0;i<10;i++) {
      //  write(fd,&arr[i],sizeof(arr[0]));
    //}

}
