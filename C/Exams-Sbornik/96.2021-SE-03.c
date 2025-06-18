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

int wrapper_lseek(int fd,off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(7,"error lseeking");
    }
    return l;
}
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) <0) {
        err(3,"error fstating");
    }
    return info.st_size;
    
}
int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(6,"Invalid number of arguments");
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(3,"Error opening");
    }
    int inputSize = getFileSize(fd);
    if(inputSize % sizeof(uint16_t) !=0) {
        errx(4,"Error file size incorrect");
    }
    if((long unsigned int)inputSize > 524288*sizeof(uint16_t)) {
        errx(5,"File too large");
    }
    int out = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(out<0) {
        err(4,"Error while opening");
    }
    const char* include = "#include <stdint.h>\n";
    wrapper_write(out,include,strlen(include));
    const char* buff = "const uint16_t arr[] = { ";
    wrapper_write(out,buff,strlen(buff));
    uint32_t arrN=1;
    uint16_t byte;
    wrapper_read(fd,&byte,sizeof(byte));

    char firstByteSave[12];
    snprintf(firstByteSave,12,"0x%x",byte);
    wrapper_write(out,firstByteSave,strlen(firstByteSave)); 
    while(wrapper_read(fd,&byte,sizeof(byte))==sizeof(byte)) {
        wrapper_write(out,", ",2);
        char byteToSave[12];
        snprintf(byteToSave,12,"0x%x",byte);
        wrapper_write(out,byteToSave,strlen(byteToSave));
        arrN++;
    }
    wrapper_write(out,"}; \n",4);
    const char* arrNText = "uint32_t arrN = ";
    wrapper_write(out,arrNText,strlen(arrNText));
    char number[12];
    snprintf(number,12,"%d;\n",arrN);
    wrapper_write(out,number,strlen(number));
    
    close(fd);
    close(out);

}
