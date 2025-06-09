#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

int wrapper_wait(void) {
    int status;
    if(wait(&status)<0) {
        err(1,"waiting error");
    }
    if(!WIFEXITED(status)) {
        err(2,"child was killed");
    }
    if(WEXITSTATUS(status)!=0) {
        err(3,"Error exit status differen than 0");
    }
    return status;
}

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
int wrapper_dup2(int old, int new) {
    int d = dup2(old,new);
    if(d<0) {
        err(5,"Error dup2");
    }
    return d;
}

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}
void handleSingleFile(const char* fileName) {
    int pf[2];
    if(pipe(pf)<0) {
        err(4,"error piping");
    }
    pid_t pid = wrapper_fork();
    if(pid==0) {
        
        close(pf[0]);
        wrapper_dup2(pf[1],1);
        close(pf[1]);

        execlp("md5sum","md5sum",fileName,(char*)NULL);
        err(4,"Error execing");
    }
    close(pf[1]);
    char fullName[1024];
    snprintf(fullName,sizeof(fullName),"%s.hash",fileName);
    //printf("%s",fullName);
    int fd = open(fullName,O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(fd<0) {
        err(12,"Error opening file %s",fullName);
    }
    char byte;
    while(wrapper_read(pf[0],&byte,sizeof(byte))==sizeof(byte)) {

        if(byte == ' ') {
            const char newLine= '\n';
            wrapper_write(fd,&newLine,sizeof(newLine));
            break;
        }
        wrapper_write(fd,&byte,sizeof(byte));
    }
    close(fd);
    close(pf[0]);
    wrapper_wait();
}
// find argv[1] -type f ! -name "*.hash"
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(3,"args mismatch");
    }
    int pf[2];
    if(pipe(pf) < 0) {
        err(4,"Error piping");
    }
    pid_t pid = wrapper_fork();
    if(pid==0) {
        close(pf[0]);
        wrapper_dup2(pf[1],1);
        close(pf[1]);
        execlp("find","find",argv[1],"-type","f","!","-name","*.hash","-printf","%p\n",(char *)NULL);
        err(6,"Error execing");

    }
    close(pf[1]);
    char fileName[4096];
    char byte;
    int idx=0;
    int count=0;
    while(wrapper_read(pf[0],&byte,sizeof(byte)) == sizeof(byte)) {

        if(byte == '\n') {
            fileName[idx]='\0';
            count++;
            //printf("%s\n",fileName);//DEBUG
            int pid2 = wrapper_fork();
            if(pid2==0) {
                handleSingleFile(fileName);
                exit(0);
            }
            idx=0;
            continue;

        }
        fileName[idx] = byte;
        idx++;
    
    }

    close(pf[0]);
    for(int i=0;i<count;i++) {
        wrapper_wait();
    }

    
}
