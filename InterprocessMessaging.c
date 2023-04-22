// InterprocessMessaging.c

#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>



// check msg lock value
int setlock(int i){
	i?system("echo \"1\" > msg.lock"):system("echo \"0\" > msg.lock");
}



// check msg lock value
int chklock(){
	return system("expr $(cat msg.lock) &> /dev/null")?0:1;
}



// set msg read status
int setread(int i){
	i?system("echo \"1\" > msg.read"):system("echo \"0\" > msg.read");
}



// check msg read status
int chkread(){
	return system("expr $(cat msg.read) &> /dev/null")?0:1;
}



int main(){
	// initialize message
	setlock(0);
	setread(1);

	// create child process and print info
	int RET=fork();
	if(RET){
		printf("PARENT PID %d    CHILD PID %d\n\n",getpid(),RET);
	}

	// parent job of sending
	if(RET){
		// msg string to send
		char wri[288],msg[256];
		while(1){
			// wait if msg locked or unread
			if(chklock()||!chkread()){
				sleep(1);
				continue;
			}
			// lock msg
			setlock(1);
			// input msg
			printf("[PARNT] Send > ");
			fgets(msg,255,stdin);
			// command for exit
			if(strcmp(msg,"/exit\n")==0){
				kill(RET,SIGKILL);
				return 0;
			}
			// write msg
			else{
				setread(0);
				msg[strlen(msg)-1]=0;
				sprintf(wri,"echo \"%s\" > msg",msg);
				system(wri);
			}
			// unlock msg
			setlock(0);
			sleep(1);
		}
	}

	//child job of receiving
	else{
		while(1){
			// wait if msg is locked or read
			if(chklock()||chkread()){
				sleep(1);
				continue;
			}
			// lock msg
			setlock(1);
			// read msg
			system("echo -n \"[CHILD] Recv > \"; cat msg");
			setread(1);
			// unlock msg
			setlock(0);
		}
	}
}
