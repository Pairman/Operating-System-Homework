// InterprocessMessaging.c



#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>



int RET=0;



/*
Set msg lock status
    param:
        (int)i: specified messaging mode, 1/0 for msg parent/child process as the sender
    return:
        (void)
*/
void setmode(int i){
	i?system("echo \"1\" > msg.mode"):system("echo \"0\" > msg.mode");
}



/*
Check msg mode status
    param:
        (void)
    return:
        (int): 1/0 for parent/child process as the sender
*/
int chkmode(){
	return system("expr $(cat msg.mode) &> /dev/null")?0:1;
}



/*
Set msg lock status
    param:
        (int)i: specified lock status, 1 or 0 for locking/unlocking
    return:
        (void)
*/
void setlock(int i){
	i?system("echo \"1\" > msg.lock"):system("echo \"0\" > msg.lock");
}



/*
Check msg lock status
    param:
        (void)
    return:
        (int): 1/0 for being locked/unlocked
*/
int chklock(){
	return system("expr $(cat msg.lock) &> /dev/null")?0:1;
}



/*
Set msg read status
    param:
        (int)i: specified read status, 1/0 for msg being read/unread
    return:
        (void)
*/
void setread(int i){
	i?system("echo \"1\" > msg.read"):system("echo \"0\" > msg.read");
}



/*
Check msg read status
    param:
        (void)
    return:
        (int): 1/0 for being read/unread
*/
int chkread(){
	return system("expr $(cat msg.read) &> /dev/null")?0:1;
}



/*
Send message via input
    param:
        (void)
    return:
        (void)
*/
void send(){
    // msg string to send
    char wri[288],msg[256];
    // lock msg
    setlock(1);
    // input msg
    chkmode()?printf("[PARNT] Send > "):printf("[CHILD] Send > ");
    fgets(msg,255,stdin);
    // command for mode switching
    if(strcmp(msg,"/mode\n")==0){
	    chkmode()?setmode(0):setmode(1);
    }
    // command for exit, only available in parent
    else if(RET&&strcmp(msg,"/exit\n")==0){
	    kill(RET,SIGKILL);
	    exit(0);
    }
    // write msg
    setread(0);
    msg[strlen(msg)-1]=0;
    sprintf(wri,"echo \"%s\" > msg",msg);
    system(wri);
    // unlock msg
    setlock(0);
}



/*
Receive message
    param:
        (void)
    return:
        (void)
*/
void recv(){
    // lock msg
    setlock(1);
    // read msg
    chkmode()?system("echo -n \"[CHILD] Recv < \"; cat msg"):system("echo -n \"[PARNT] Recv < \"; cat msg");
    setread(1);
    // unlock msg
    setlock(0);
}



int main(){
	// initialize message file
	setmode(1);
	setlock(0);
	setread(1);

	// create child process
	RET=fork();

	// parent job
	if(RET){
        	// print process info first
        	printf("PARENT PID %d    CHILD PID %d\n\n",getpid(),RET);
		while(1){
            		// parent as the sender
            		if(chkmode()){
                		// wait if msg locked or unread
                		if(chklock()||!chkread()){
                    			sleep(1);
                		}
                		else{
					// send message
					send();
				}
			}
			// parent as the receiver
			else{
				// wait if msg is locked or read
				if(chklock()||chkread()){
					sleep(1);
				}
				else{
					recv();
				}
			}
			sleep(1);
		}
	}
	//child job
	else{
        sleep(1);
		while(1){
            	// child as the receiver
            		if(chkmode()){
			// wait if msg is locked or read
                		if(chklock()||chkread()){
                    			sleep(1);
                			}
                		else{
                			recv();
            			}
            		}
            		// child as the sender
            		else{
                		// wait if msg locked or unread
                		if(chklock()||!chkread()){
                    			sleep(1);
                		}
                		else{
                    			// send message
                    			send();
                		}
            		}
		sleep(1);
        	}
	}
}
