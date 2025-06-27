#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>

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

pid_t wrapper_fork(void) {

    pid_t pid = fork();
   if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
}

typedef struct Program {
    char * name;
    int pid;
    time_t start;
    int index;
    int alive;
} Program;

Program programs[10];


int handleProgram(const char* name) {
    int pid = wrapper_fork();
    if(pid==0) {
        execlp(name,name,(char*)NULL);
        err(30,"Error on execlp");
    }
    return pid;
}

int findProgram(int pid) {
    for(int i=0;i<10;i++) {
        if(programs[i].pid==pid) {
            return i;
        }
    }
    return -1;
}
void saveToFile(int fd, int isTimeouted, int programIndex) {
    time_t currTime = time(NULL);
    struct tm* startTime = localtime(&programs[programIndex].start);
    struct tm copyStart = *startTime; // localtime spodelq 1 ukazatel i ne se zapisva starta
    struct tm* endTime = localtime(&currTime);
    char startTimeBuffer[1024];
    char endTimeBuffer[1024];
    strftime(startTimeBuffer,1024,"%F %T",&copyStart);
    strftime(endTimeBuffer,1024,"%F %T", endTime);
    char mainBuffer[4096];
    if(isTimeouted==0) {
        snprintf(mainBuffer,4096,"Program %s Started: %s  Ended: %s   without timeout!!\n",programs[programIndex].name,startTimeBuffer,endTimeBuffer);
    }
    else {
       snprintf(mainBuffer,4096,"Program %s Started: %s  Ended: %s   with timeout OR was KILLED!!\n",programs[programIndex].name,startTimeBuffer,endTimeBuffer); 
    }
    wrapper_write(fd,mainBuffer,strlen(mainBuffer));


}
int main(int argc, char* argv[]) {
    if(argc>10 || argc<2) {
        errx(3,"Invalid arguments");
    }
    int timeOut = 5; // 5 sekundi timeout
    
    for(int i=1;i<argc;i++) {
        int pid = handleProgram(argv[i]);
        Program current;
        current.name = argv[i];
        current.pid = pid;
        current.start = time(NULL);
        current.index=i;
        current.alive=1;
        programs[i-1] =current;
    }
    int log = open("myLog.log",O_WRONLY | O_CREAT | O_TRUNC,0666);
    if(log<0) {
        err(4,"Error on open");
    }

    while(true) {
        int donePid;
        int status;
        while((donePid = waitpid(-1,&status,WNOHANG)) > 0) {
            int index = findProgram(donePid);
            if(programs[index].alive ==0) break;
            programs[index].alive=0;
            saveToFile(log,0,index);
            if(WIFSIGNALED(status)) {// kill All processes
                for(int i=0;i<argc-1;i++) {
                    if(programs[i].alive!=0) {
                        saveToFile(log,1,i);
                        if(kill(programs[i].pid,SIGTERM)<0) {
                            err(14,"Error on killing");
                        }
                    }
                }
                exit(1); // exit 1 killed all processes
            }
            if(WEXITSTATUS(status) !=0) { // start proccess again
                printf("Vlizame li tuka\n");
                programs[index].alive=1;
                programs[index].start = time(NULL);
                programs[index].pid=handleProgram(programs[index].name);
            }
            

        }
        if(donePid==-1) {
            break;
        }
        time_t currentTime = time(NULL);
        for(int i=0;i<argc-1;i++) {
            if(currentTime - programs[i].start > timeOut && programs[i].alive) {
                programs[i].alive=0;
                saveToFile(log,1,i);
                if(kill(programs[i].pid,SIGTERM)<0) {
                    err(14,"Error on kill");
                }
                waitpid(programs[i].pid,NULL,0); // zaradi zombi moje da se sloji 
                programs[i].alive=0;
            }
            
        }
    }
    close(log);

}
