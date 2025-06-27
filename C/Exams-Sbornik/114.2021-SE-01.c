#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <pwd.h>
#include <sys/types.h>

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
void lock(const char* name,int fdToClose) {
    
    int pid = wrapper_fork();
    if(pid==0) {
        close(fdToClose);
        execlp("echo","echo","passwd","-i",name,(char*)NULL);
        err(5,"Error on passwd -i");
    }

    wrapper_wait();

}
void terminate(const char* name,int fdToClose) {
    int pid = wrapper_fork();
    if(pid==0) {
        close(fdToClose);
        execlp("echo","echo","killall","-u",name,(char*)NULL);
        err(5,"Error on killAlll");
    }

    wrapper_wait();

}
void execute(char * argv[],int fdToClose) {
    int pid = wrapper_fork();
    if(pid==0) {
        close(fdToClose);
        execvp(argv[1],argv+1);
        err(5,"Error on execvp");
    }

    wrapper_wait();

}
int main(int argc, char* argv[]) {
    struct timeval timeInfo;
    if (gettimeofday(&timeInfo,NULL)<0) {
        err(3,"Error geting time");
    }
    //printf("%ld\n",timeInfo.tv_sec);
    //printf("%ld\n",timeInfo.tv_usec); 
    struct tm* timeStruct=localtime(&timeInfo.tv_sec);
    if(timeStruct==NULL) {
        errx(4,"Error on getting time");
    }
    char buffer[4096];
    strftime(buffer,4096,"%F %T.",timeStruct);
    char microseconds[12];

    snprintf(microseconds,12,"%ld",timeInfo.tv_usec); // must be converted to char
    strcat(buffer,microseconds);
    
    int userID = getuid();
    //printf("%d\n",userID);

    //geting this struct for the name
    struct passwd* pwInfo = getpwuid(userID);
    if(pwInfo==NULL) {
        errx(12,"User Not Found");
    }
    const char* empty = " ";
    //puting the name
    strcat(buffer,empty);
    strcat(buffer,pwInfo->pw_name);
    //putting all the commands
    for(int i=0;i<argc;i++) {
        strcat(buffer,empty);
        strcat(buffer,argv[i]);
    }
    //printf("%s\n",pwInfo->pw_name);
    strcat(buffer,"\n");
    int foo = open("foo.log",O_WRONLY | O_CREAT |O_APPEND,0666);
    if(foo<0) {
        err(12,"Error on open!!!");
    }

    wrapper_write(foo,buffer,strlen(buffer));
    //char* myArgv[100];
    //int j=0;
    //for(;j<argc;j++) {
      //  myArgv[j] =argv[j];
    //}
    //myArgv[j] = NULL;
    execute(argv,foo);
    lock(pwInfo->pw_name,foo);
    terminate(pwInfo->pw_name,foo);
    close(foo);
    
}
