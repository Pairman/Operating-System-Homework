// InterprocessMessaging-pipe.c



#include<fcntl.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>



#define PIPE_BUF 256



int main(){
    // create fds, pipes and msg buffer, fd1/fd0 for parent/child sending
    int fd0[2],fd1[2],msglen=0;
	pipe(fd0);
	pipe(fd1);
    char msg[PIPE_BUF];

	// create child process and initialize mode
	int RET=fork(),MODE=1;

	// parent job
	if(RET){
		// print process info first
		printf("PARENT PID %d    CHILD PID %d\n\n",getpid(),RET);
		while(1){
			// parent as the sender
			if(MODE){
				// input msg
				printf("[PARNT] Send > ");
				fgets(msg,PIPE_BUF-1,stdin);
				// send msg over pipe fd1: close the read end, open and write to the write end
				close(fd1[0]);
				write(fd1[1],msg,strlen(msg));
				// command for mode switching
				if(strcmp(msg,"/mode\n")==0){
					MODE=1-MODE;
					sleep(2);
				}
				// command for exit
				else if(strcmp(msg,"/exit\n")==0){
					exit(0);
				}

			}
			// parent as the receiver
			else{
				// reveice msg over pipe fd0: close the write end, open and read from the read end
				close(fd0[1]);
				msglen=read(fd0[0],msg,PIPE_BUF);
				// read msg
				msg[msglen]=0;
				printf("[PARNT] Recv < %s",msg);
				// check for mode switching
				if(strcmp(msg,"/mode\n")==0){
					MODE=1-MODE;
					sleep(2);
				}
				// check for exit
				else if(strcmp(msg,"/exit\n")==0){
					close(fd0[0]);
					close(fd0[1]);
					close(fd1[0]);
					close(fd1[1]);
					exit(0);
				}
			}
			sleep(1);
		}
	}
	//child job
	else{
		while(1){
			// child as the receiver
			if(MODE){
				// reveice msg over pipe fd1: close the write end, open and read from the read end
				close(fd1[1]);
				msglen=read(fd1[0],msg,PIPE_BUF);
				// read msg
				msg[msglen]=0;
				printf("[CHILD] Recv < %s",msg);
				// check for mode switching
				if(strcmp(msg,"/mode\n")==0){
					MODE=1-MODE;
					sleep(2);
				}
				// check for exit
				else if(strcmp(msg,"/exit\n")==0){
					close(fd0[0]);
					close(fd0[1]);
					close(fd1[0]);
					close(fd1[1]);
					exit(0);
				}
			}
			// child as the sender
			else{
				// input msg
				printf("[CHILD] Send > ");
				fgets(msg,PIPE_BUF-1,stdin);
				// send msg over pipe fd0: close the read end, open and write to the write end
				close(fd0[0]);
				write(fd0[1],msg,strlen(msg));
				// command for mode switching
				if(strcmp(msg,"/mode\n")==0){
					MODE=1-MODE;
					sleep(2);
				}
				// command for exit
				else if(strcmp(msg,"/exit\n")==0){
					exit(0);
				}
			}
			sleep(1);
		}
	}
}
