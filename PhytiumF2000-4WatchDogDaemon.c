#include<stdio.h>  
#include<sys/types.h>  
#include<sys/stat.h>  
#include<fcntl.h>  
#include<sys/ioctl.h>  



// define operations
#define WDT_ENABLE 1
#define WDT_UPDATE 2



int main(int argc,char*argv[]){  
    int fd;
    fd=open("/dev/wdt",O_RDWR);
    if(fd<0){  
        perror("open");
        return -1;
    }

    // enable wdt
    ioctl(fd,WDT_ENABLE);

    // update wdt
    while(1){
        ioctl(fd,WDT_UPDATE);
    }

    return 0;  
}  