#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int wrapper_wait(void) {
    int status;
    if(wait(&status)==-1) {
        err(1,"waiting error");
    }
    if(!WIFEXITED(status)) {
        errx(2,"child was killed");
    }
    //if(WEXITSTATUS(status)!=0) {
      //  errx(3,"Error exit status differen than 0");
    //}
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
    int d = dup2(f1,f2);
    if(d<0) {
        err(26,"Dupping");
    }
    return d;
}

int pids[1000];
int size=0;

int main(int argc, char* argv[]) {

    int signal[2];
    if(pipe(signal) <0) {
        err(26,"Error piping");
    }

    for(int i=1;i<argc;i++) {
        int pid2=fork();
        if(pid2<0) {
            err(26,"Error forking");
        }
        if(pid2==0) {
            close(signal[0]);
            wrapper_dup2(signal[1],1);
            close(signal[1]);
            execlp(argv[i],argv[i],(char*)NULL);
            err(26,"Error execing");

        }
        pids[size] = pid2;
        size++;
     }
     close(signal[1]);
     char buffer[5000];
     char ch;
        int idx=0;
        while(wrapper_read(signal[0],&ch,sizeof(ch)>0)) {
            if(ch=='\n') {
                buffer[idx]='\0';
                if(strcmp(buffer,"found it!")==0) {
                    printf("Size: %d\n",size);
                    for(int j=0;j<size;j++) {
                        printf("I am killing %d\n",pids[j]);
                        if(kill(pids[j],15) == -1) {
                            err(26,"Error kiling");
                        }
                    }
                    exit(0);
                }
                idx=0;
                continue;
            }
            buffer[idx]=ch;
            idx++;
    }
    close(signal[0]);
    for(int i=1;i<argc;i++) {
        wrapper_wait();
    }
    exit(1);
}
