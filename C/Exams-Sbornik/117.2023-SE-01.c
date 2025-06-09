#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int wrapper_wait(void) {
    int status;
    if(wait(&status)==-1) {
        err(1,"waiting error");
    }
    if(!WIFEXITED(status)) {
        if(WIFSIGNALED(status)) {
            printf("%d\n",WTERMSIG(status));
        }
        errx(2,"child was killed");
    }
    if(WEXITSTATUS(status)!=0) {
        errx(3,"Error exit status differen than 0");
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
int wrapper_dup2(int f1, int f2) {
    int d=dup2(f1,f2);
    if(d<0) {
        err(9,"dup");
    }
    return d;
}
void createFile(const char* buffer) {
    int p2[2];
    if(pipe(p2)<0) {
        err(6,"Error piping");
    }
    int pid2=fork();
    if(pid2<0) {
        err(8,"Error forking");
    }
    printf("dasdsasd %d\n",pid2);
    if(pid2==0) {
        close(p2[0]);
        wrapper_dup2(p2[1],1);
        close(p2[1]);
        execlp("md5sum","md5sum",buffer,(char*)NULL);
        err(10,"error execing");
    }
    close(p2[1]);
    char hashBuffer[32+1];
    wrapper_read(p2[0],hashBuffer,32);
    close(p2[0]);
    hashBuffer[32]='\0';
    char fileName[5000];
    snprintf(fileName,sizeof(fileName),"%s.hash",buffer);
        
    int fd1= open(fileName,O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd1<0) {
        err(11,"Error fd1");
    }
    const char newLine='\n';
    wrapper_write(fd1,hashBuffer,sizeof(hashBuffer));
    wrapper_write(fd1,&newLine,sizeof(newLine));
    
    wrapper_wait();

}
int main(int argc, char* argv[]) {
    if(argc!=2) {
        err(5,"Error arguments");
    }
    int p1[2];
    if(pipe(p1) == -1 ) {
        err(6,"Err piping");
    }
    int pid1=fork();
    if(pid1 <0) {
        err(5,"err forking");
    }
    
    printf("%d\n",pid1);
    if(pid1==0) {
        close(p1[0]);
        wrapper_dup2(p1[1],1);
        close(p1[1]);
        if(execlp("find","find",argv[1],"-type","f","!","-name","*.hash",(char*)NULL) == -1 ) {
            err(31,"Error find");
        }
    }
    close(p1[1]);
    char buffer[4096];
    char ch;
    int idx=0;
    int counter=0;
    while(wrapper_read(p1[0],&ch,sizeof(ch)) > 0) {
        if(ch=='\n') {
            buffer[idx]='\0';
            //printf("%s\n",buffer);
            int pid3=fork();
            if(pid3<0) {
                err(131,"Error forking");
            }
            if(pid3==0) {
                createFile(buffer);
                exit(0);
            }
            counter++;
            idx=0;
            continue;
        }
        buffer[idx++] = ch;
    }
    close(p1[0]);
    wrapper_wait();
    for(int i=0;i<counter;i++) {
        wrapper_wait();
    }
}
