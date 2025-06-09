#include <sys/wait.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

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

off_t wrapper_lseek(int fd, off_t offset, int whence) {
    off_t l = lseek(fd,offset,whence);
    if(l<0) {
        err(4,"Error lseeking");
    }
    return l;
    
}
typedef struct Five {
    uint32_t uid;
    uint16_t temp1;
    uint16_t temp2;
    uint32_t start;
    uint32_t end;

} Five;

int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) < 0) {
        err(10,"error fstating");
    }
    return info.st_size;
}

uint32_t calculateAll(int fd) {

    uint32_t res=0;
    Five five;
    while(wrapper_read(fd,&five,sizeof(five)) == sizeof(five)) {
        uint32_t currentTime = five.end - five.start;
        res+=currentTime;
    }
    wrapper_lseek(fd,0,SEEK_SET);
    return res;
}
uint32_t calculateDispersion(int fd, uint32_t middleValue,int totalSessions) {
    
    uint32_t res=0;
    Five five;
    while(wrapper_read(fd,&five,sizeof(five))==sizeof(five)) {
        uint32_t currentTime=five.end-five.start;
        uint32_t kvadrat = (currentTime - middleValue) * (currentTime - middleValue) ;
        res+=kvadrat;
    }

    res = res/totalSessions;
    wrapper_lseek(fd,0,SEEK_SET);
    return res;
}
typedef struct People{
    uint32_t uid;
    uint32_t time;

} People;
//magic number FIX !!
int findPeopleByUID(People peoples[2048], uint32_t uid,int size) {
    for(int i=0;i<size;i++) {
        if(peoples[i].uid==uid) {
            return i;
        }
    }
    return -1;
    
}
int main(int argc, char* argv[]) {

    if(argc!=2) {
        errx(1,"Error arguments mismatch");
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(2,"Error opening file");
    }
    int fileSize = getFileSize(fd);
    if(fileSize%sizeof(Five)!=0) {
        errx(3,"Incorrect File Size");
    }
    int totalSessions = fileSize / sizeof(Five);
    uint32_t allTime = calculateAll(fd);
    uint32_t middleValue = allTime / totalSessions;
    uint32_t dispersion = calculateDispersion(fd,middleValue,totalSessions);
    
    People people[2048] = {0};
    Five five;
    int size=0;
    while(wrapper_read(fd,&five,sizeof(five)) == sizeof(five)) {
        uint32_t time = five.end - five.start;
        int pIndex = findPeopleByUID(people,five.uid,size);
        if(pIndex == -1) {
            people[size].uid = five.uid;
            people[size].time = time;
            size++;
        }
        else {
            if(people[pIndex].time<time) {
                people[pIndex].time=time;
            }
        }
        
    }
    for(int i=0;i<size;i++) {
        //wrapper_write(1,&people[i].time,sizeof(people[i].time));
        if((people[i].time*people[i].time) > dispersion) {
            char buff[1024];
            snprintf(buff,sizeof(buff),"UID : %u Time: %u \n",people[i].uid,people[i].time);
            wrapper_write(1,buff,strlen(buff));

        }
        
    }
    close(fd);

}

