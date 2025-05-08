//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.
#include <err.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include<ctype.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
int wrapped_read(int fd, void* buff, size_t size) {
    int r = read(fd,buff,size);
    if( r < 0 ) {
        err(3,"failed to read %d",fd);
    }
    return r;
}
int wrapped_write(int fd, const void* buffer, size_t size) {
    int w= write(fd,buffer,size);
    
    if(w<0) {
        err(4,"failoted to write %d",fd);
    
    }
    return w;
}
off_t wrapped_lseek(int fd, off_t offset, int whence) {
    off_t r = lseek(fd,offset,whence);
    if(r<0) {
        err(5,"failed to lseek");
    } 
    return r;
}
int wrapper_open(const char* pathname, int flags, mode_t* access) {
    int fd;
    if(access == NULL) {
        fd = open(pathname,flags);
    }
    else{
        fd=open(pathname,flags,*access);
    }
    if(fd < 0) {
        err(2,"failed to open %s",pathname);
    }
    return fd;

}

int main(int argc, char* argv[]) {
    
    //printf("e: %d\n",argc);
    if(argc<3) {
        errx(1,"Error arguments");
    }
    for(int i=1;i<argc-1;i++) {

    int fd1 = open(argv[i], O_RDONLY);
    if(fd1<0){
        err(4,"cannot open");
    }
    struct stat info;
    char fileName[1024];
    snprintf(fileName,1024,"%s%s%s",
    argv[argc-1],
    argv[argc-1][strlen(argv[argc-1]) -1 ] != '/' ? "/" : "",
                                    argv[i]);
    int result = stat(argv[argc-1],&info);
    if(result == -1 ) {
        err(5,"stat error");
    }
    if(! S_ISDIR(info.st_mode) ) {
        errx(6,"%s is not directory",argv[argc-1]);
    }
    int fd2=open(fileName,O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(fd2==-1) {
        err(3,"Error");
    }
    char c;
    while((wrapped_read(fd1,&c,sizeof(c))) == sizeof(c)) {
        wrapped_write(fd2,&c,sizeof(c));
        
    }
        close(fd1);
        close(fd2);
    
    }

}
