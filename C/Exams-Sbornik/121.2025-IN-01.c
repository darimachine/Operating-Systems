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
pid_t wrapper_fork(void) {

    pid_t pid = fork();
    if(pid<0) {
        err(6,"Error forking");
    }
    return pid;
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
typedef struct Triple {
    uint16_t ram_size;
    uint16_t register_count;
    char filename[8];
} Triple;

void handleProcess(Triple* t,int fdToClose) {
    
    int currFd = open(t->filename,O_RDWR);
    pid_t pid = wrapper_fork();
    if(pid == 0) {

        close(fdToClose);
        printf("Idvame li tuka s koy fail: %s\n",t->filename);

        uint8_t registers[32];
        uint8_t ramInit[512];
        uint8_t instructions[4];
        
        wrapper_read(currFd,registers,t->register_count);
        wrapper_read(currFd,ramInit,t->ram_size);

        while(wrapper_read(currFd,instructions,4)  == 4) {
            uint8_t opcode = instructions[0];
            uint8_t op1 = instructions[1];
            uint8_t op2 = instructions[2];
            uint8_t op3 = instructions[3];
            printf("day mi opcode: %d\n",opcode);
            if(opcode ==0) {
                registers[op1] = registers[op2] & registers[op3];
            }
            else if(opcode==1) {
                registers[op1] = registers[op2] | registers[op3];
            }
            else if(opcode==2) {
                registers[op1] = registers[op2] + registers[op3]; 
            }
            else if(opcode==3) {
                registers[op1] = registers[op2] * registers[op3]; 
            }
            else if(opcode==4) {
                registers[op1] = registers[op2] ^ registers[op3]; 
            }
            else if(opcode==5) {
                wrapper_write(1,&registers[op1],1);
            }
            else if(opcode==6) {
                printf("Kolko sekundi: %d\n",registers[op1]);
                sleep(registers[op1]);
            }
            else if(opcode==7) {
                registers[op1] = ramInit[registers[op2]];
            }
            else if(opcode==8) {
                ramInit[registers[op2]] = registers[op1];
            }
            else if(opcode==9) {
                if(registers[op1] != registers[op2]) {
                    
                    wrapper_lseek(currFd,(t->register_count+t->ram_size) + op3*4,SEEK_SET);    
                }
            }
            else if(opcode==10) {
                registers[op1] = op2;
            }
            else if(opcode==11) {
                ramInit[registers[op1]] =op2;
            }
            else {
                errx(12,"Incorrect opcode %d",opcode);
            }
        }
        wrapper_lseek(currFd,0,SEEK_SET);
        wrapper_write(currFd,registers,t->register_count);
        wrapper_write(currFd,ramInit,t->ram_size);
        close(currFd);

        exit(0);
    }
    close(currFd);
}
int main(int argc, char* argv[]) {
    if(argc!=2) {
        errx(3,"incorrect number of args");
    }
    int fd = open(argv[1],O_RDONLY);
    if(fd<0) {
        err(3,"Error opening this file");
    }
    int size = getFileSize(fd);
    if(size % sizeof(Triple) !=0) {
        errx(21,"Error incorrect file size");
    }
    Triple t;
    int count=0;
    while(wrapper_read(fd,&t,sizeof(t)) == sizeof(t)) {
        if(t.ram_size > 512) {
            errx(12,"Too much ram size overflow");
        }
        if(t.register_count > 32) {
            errx(13,"Too much register count");
        }
        count++;
        handleProcess(&t,fd);
        
    }

    for(int i=0;i<count;i++) {
        wrapper_wait();
    }
    close(fd);

}
