#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

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

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}
void run(const char* command) {
    char token[5];
    int tlen=0;
    char cmdArgs[5][2+1];
    int args=0;
    char c;
    while(wrapper_read(0,&c,1) == 1) {
        
        if(c==' ' || c=='\n') {
            
            token[tlen] = '\0';
            //printf("%s Dali si tuk\n",token);
            //printf("Char: %d\n",c);
            strcpy(cmdArgs[args],token);
            args++;
            tlen=0;
            if(args==2) {
                    //printf("DEBUG : %s  %s\n",cmdArgs[0],cmdArgs[1]); // debug
                    pid_t pid = wrapper_fork();
                    if(pid==0) {
                        
                        execlp(command,command,cmdArgs[0],cmdArgs[1],(char*)NULL);
                        err(2,"Error execing");
                    }
                    args=0;
                    wrapper_wait();
            }
        }
        else{
            if(tlen>=4) {
                errx(3,"Token too long error max must be 4");
            }
            token[tlen]=c;
            tlen++;
        }

    }
    if(args==1) {//edge case f1
        //printf("Vlizash li tuk %s\n",token);
        pid_t pid = wrapper_fork();
        if(pid==0) {
            execlp(command,command,cmdArgs[0],(char*)NULL);
            err(2,"Error execing");
        }
        wrapper_wait();
    }
    else if (args==0 && tlen>0) { // if there is only 1 command Example: f2
        token[tlen] = '\0';
        //printf("%s  pisnaMI\n",token);
        strcpy(cmdArgs[args], token);
        tlen=0;
        pid_t pid = wrapper_fork();
        if(pid==0) {
            execlp(command,command,cmdArgs[0],(char*)NULL);
            err(2,"Error execing");
        }
        wrapper_wait();
    }

}
int main(int argc, char* argv[]) {
    if(argc==1) {
        run("echo");
    }
    else if(argc==2) {
        if(strlen(argv[1])>4) {
            errx(2,"Maximum command length must be 4");
        }
        run(argv[1]);

    }
    else{
        errx(1,"Invalid arguments");
    }
    
}
