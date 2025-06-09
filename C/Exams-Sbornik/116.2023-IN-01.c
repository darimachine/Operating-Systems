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
        //printf("%d\n",WEXITSTATUS(status)); TESTING
        //printf("%d\n",WTERMSIG(status)); TESTING
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
const char* words[] = {"tic ","tac ","toe\n"};
int nc;
int wc;
int pipes[8][2];

void closePipes(int write, int read) {
    for(int i=0;i<nc+1;i++) {
        if(i!=write) {
           close(pipes[i][1]);
        }
        if(i!=read) {
            close(pipes[i][0]);
        }
    }
}
void runChild(int next) {
    
    closePipes((next+1)%(nc+1),next);
    
    int idx;
    while(wrapper_read(pipes[next][0],&idx,sizeof(idx))==sizeof(idx)) {

        if(idx>=wc) {
            //DONT UNCOMMENT BUG SIGPIPE HERE FOR TESTING ONLY!!!
            //close(pipes[number][0]);
            //wrapper_write(pipes[(number+1)%(nc+1)][1],&idx,sizeof(idx))
            //close(pipes[(number+1)%(nc+1)][1]);
            break;
        }

        wrapper_write(1,words[idx%3],4);
        idx++;
        wrapper_write(pipes[(next+1) %(nc+1)][1],&idx,sizeof(idx));
    }
    close(pipes[next][0]);
    close(pipes[(next+1)%(nc+1)][1]);
}
        
int main(int argc, char* argv[]) {
    if(argc!=3) {
        errx(5,"ärguments mismatc");
    }
    nc = atoi(argv[1]);
    if(nc<1 || nc > 7) {
        errx(10,"Incorrect nc argument");
    }
    wc = atoi(argv[2]);
    if(wc<1 || wc > 35) {
        errx(11,"Incorrect wc argument");
    }
    for(int i=0;i<nc+1;i++) {
        if(pipe(pipes[i]) < 0) {
            err(5,"Error on piping");
        }
    }

    for(int i=0;i<nc;i++) {
        int pid = wrapper_fork();
        if(pid==0) {
            runChild(i);
            exit(0);
        }
    }
    
    
    int idx=0;
    wrapper_write(1,words[idx],4);
    idx++;
    wrapper_write(pipes[0][1],&idx,sizeof(idx));
    runChild(nc);
    
    for(int j=0; j<nc; j++) {
        wrapper_wait();
    }


}

