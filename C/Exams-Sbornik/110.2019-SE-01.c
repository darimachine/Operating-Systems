#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
// TESTED WITH BASH SCRIPT WORKS!!
int wrapper_write(int fd, const void * buff, int size) {
    int w = write(fd,buff,size);
    if(w<0) {
        err(4,"Error writing");
    }
    return w;
}

pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}

int main(int argc, char* argv[]) {
    if(argc<3) {
        errx(2,"Incorrect number of arguments");
    }
    uint8_t seconds = strtol(argv[1],NULL,10);
    if(seconds < 1 || seconds > 9) {
        errx(1,"Invalid first arguments");
    }
    int i=3;
    char* params[1000];
    params[i-3] = argv[2]; // first arg the name of the command
    for(;i<argc;i++){
        params[i-2] = argv[i];
    }
    params[i-2]= NULL; // for i=3 check!
    printf("%s\n",params[0]);
    int fd = open("run.log",O_CREAT | O_TRUNC | O_WRONLY,0666);
    if(fd<0) {
        err(4,"Error opening file");
    }
    int count=0;
    int exitCode;
    while(true) {
        pid_t pid = wrapper_fork();
        time_t start =time(NULL);
        if(pid==0) {
            close(fd);
            execvp(argv[2],params);
            err(4,"Error execing");
        }
        int status;
        if(wait(&status) < 0) {
            err(5,"Error on wait");
        }
        time_t end = time(NULL);
        if(WIFSIGNALED(status)) {
            exitCode=129;
        }
        else{
            exitCode = WEXITSTATUS(status);
        }   
        char buffer[1024];
        snprintf(buffer,sizeof(buffer),"%ld %ld %d\n",start,end,exitCode);
        wrapper_write(fd,buffer,strlen(buffer));
        if(exitCode !=0 && (end -start) < seconds) {
            count++;
            if(count==2) {
                break;
            }
        }
        else {
            count=0;
        }
    
    }
    close(fd);

}
