//Koпирайте файл /etc/passwd в текущата ви работна директория и променете разделителят на копирания файл от ":", на "?"
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>


int wrapped_read(int fd,void* buff,int size) {
    int r=read(fd,buff,size);
    if(r<0) {
        err(8,"Error reading file");
    }
    return r;
}

int main(void) {
    
    int fd1=open("passwd",O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(fd1<0) {
        err(1,"Erorr creating file");
    }
    int fd2=open("/etc/passwd",O_RDONLY);
    if(fd2<0) {
        err(2,"Error openign etc/passwd");
    }
    char c;
    int res;
    while((res=wrapped_read(fd2,&c,sizeof(c))) > 0) {
        
        if(c==':') {
            c='?';
        }

        if(write(fd1,&c,res) < 0) {
           err(3,"Error while writing"); 
        }
    }

    close(fd1);
    close(fd2);








}

