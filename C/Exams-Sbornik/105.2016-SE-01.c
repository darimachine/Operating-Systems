#include <err.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>



int wrapper_write(int fd,const void* buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(4,"Err writing");
    }
    return w;
}

int wrapper_read(int fd,void* buff, int size) {
    int r = read(fd,buff,size);
    if(r<0) {
        err(5,"Err reading");
    }
    return r;
}
int wrapper_dup2(int old, int new) {
    int res = dup2(old,new);
    if(res<0){
        err(6,"dup2Error");
    }
    return res;
}

pid_t wrapper_fork(void) {
    pid_t id = fork();
    if(id<0) {
        err(7,"err forking");
    }
    return id;
}

int main(int argc,char* argv[]) {

    if(argc!=2) {
        err(1,"Err args");
    }
    int p1[2];
    
    if(pipe(p1)==-1) {
        err(2,"Err piping");
    }

    pid_t id = wrapper_fork();
    
    if(id==0) {
        close(p1[0]);
        wrapper_dup2(p1[1],1);
        close(p1[1]);

        if(execlp("cat","cat",argv[1],(char*)NULL) == -1 ) {
            err(4,"error on execlp");
        }
    }
    int p2[2];
    if(pipe(p2) == -1 ) {
        err(3,"Err piping");
    }

    close(p1[1]);

    pid_t id2= wrapper_fork();
    // added wc command
    if(id2==0) {
        wrapper_dup2(p1[0],0);
        close(p1[0]);

        close(p2[0]);
        wrapper_dup2(p2[1],1);
        close(p2[1]);
        if(execlp("sort","sort","-nr",(char *)NULL) == -1) {
            err(8,"eror exec");
        }
    }
    close(p1[0]);
    
    close(p2[1]);
    wrapper_dup2(p2[0],0);
    close(p2[0]);

    if(execlp("wc","wc",(char *)NULL) == -1) {
        err(8,"eror exec");
    }
    
}
