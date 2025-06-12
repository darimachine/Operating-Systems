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

int wrapper_lseek(int fd,off_t offset, int whence) {
    int l = lseek(fd,offset,whence);
    if(l<0) {
        err(7,"error lseeking");
    }
    return l;
}
int getFileSize(int fd) {
    struct stat info;
    if(fstat(fd,&info) <0) {
        err(3,"error fstating");
    }
    return info.st_size;
    
}
typedef struct Header {
    char filler[4];
    uint16_t count;
    char filler2[10];
} Header;
// affix = uint16_t elementi
// infix = uint16_t elementi
// prefix = uint8_t
// crucifixus = uint8_t
//postfix = uint32_t
// suffix = uint64_t

typedef struct Interval{
 uint16_t offset;
 uint16_t count;
} Interval;
//0 = uint32_t,  1= uint8_t, 2=uint16_t, 3=uint64_t
int countElements(int affix) {
    int i=0;
    uint16_t res=0;
    Interval interval;
    Header h;
    wrapper_read(affix,&h,sizeof(h)); // skiping header
    while(wrapper_read(affix,&interval,sizeof(interval)) == sizeof(interval)) {
        if(i==0) {
            res+=interval.count * sizeof(uint32_t);
        }
        else if(i==1) {
            res+=interval.count* sizeof(uint8_t);
        }
        else if(i==2) {
            res+=interval.count*sizeof(uint16_t);
        }
        else if(i==3) {
            res+=interval.count*sizeof(uint64_t);
        }
        i++;
        i=i%4;
    }
    wrapper_lseek(affix,0,SEEK_SET);
    return res;
}
int main(int argc, char * argv[]) {
    if(argc !=7) {
        errx(2,"Error argument mismatch");
    }
    int affix = open(argv[1],O_RDONLY);
    if(affix<0) {
        err(3,"Error opening affix");
    }
    if(getFileSize(affix) % sizeof(uint16_t) !=0) {
        errx(4,"Invalid file affix");
    }
    int postfix = open(argv[2],O_RDONLY); // err
    int prefix= open(argv[3],O_RDONLY); // err
    int infix= open(argv[4],O_RDONLY); // err
    int suffix= open(argv[5],O_RDONLY); // err
    int crucifixus = open(argv[6],O_WRONLY | O_CREAT | O_TRUNC,0666);
    int count = countElements(affix);
    Header h;
    wrapper_read(affix,&h,sizeof(h));
    Header toSave;
    toSave.count=count;
    wrapper_write(crucifixus,&toSave,sizeof(toSave));
    
    int idx=0;
    Interval interval;
    while(wrapper_read(affix,&interval,sizeof(interval)) == sizeof(interval)) {
        if(idx==0) {
            //post
            wrapper_lseek(postfix,interval.offset*sizeof(uint32_t) + sizeof(Header),SEEK_SET);
            for(int i=0;i<interval.count;i++) {
                uint32_t byte;
                wrapper_read(postfix,&byte,sizeof(byte));
                wrapper_write(crucifixus,&byte,sizeof(byte));
            }
        }
        else if(idx==1) {
            //pre
            wrapper_lseek(prefix,interval.offset*sizeof(uint8_t)+sizeof(Header),SEEK_SET);
            for(int i=0;i<interval.count;i++) {
                uint8_t byte;
                wrapper_read(prefix,&byte,sizeof(byte));
                wrapper_write(crucifixus,&byte,sizeof(byte));
            }    

        }
        else if(idx==2) {
            //infix
            wrapper_lseek(infix,interval.offset*sizeof(uint16_t) + sizeof(Header),SEEK_SET);
            for(int i=0;i<interval.count;i++) {
                uint16_t byte;
                wrapper_read(infix,&byte,sizeof(byte));
                wrapper_write(crucifixus,&byte,sizeof(byte));
            }    
            
        }
        else if(idx==3) {
            //suffix
            wrapper_lseek(suffix,interval.offset*sizeof(uint64_t) + sizeof(Header),SEEK_SET);
            for(int i=0;i<interval.count;i++) {
                uint64_t byte;
                wrapper_read(suffix,&byte,sizeof(byte));
                wrapper_write(crucifixus,&byte,sizeof(byte));
            }    
            
        }
        idx++;
        idx=idx%4;
    }
    

    close(affix);
    close(postfix);
    close(prefix);
    close(infix);
    close(suffix);
    close(crucifixus);
}
