#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	mqd_t mq;
	struct mq_attr attr;
	pid_t p;
	int t;
	int secret;
	char *QUEUE_NAME = ".queue";
	
	if (argc != 2) {
		printf("Usage: %s QUEUE_NAME\n",argv[0]);
		return 0;
	}
	
	QUEUE_NAME = argv[1];
	
	/* initialize the queue attributes */
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(secret);
	attr.mq_curmsgs = 0;

	/* create the message queue */
	if(mq == -1){
		
	}

	p = fork();
	if(p>0){

		srand(time(0));
		secret = rand();

		mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0644, &attr);
		mq_send(mq, &secret, sizeof(secret), 0));
		
		fprintf(stdout,"parent: secret=%8d\n",secret);
		wait(&t);
		
		mq_close(mq);
		mq_unlink(QUEUE_NAME));
	}else if(p==0){
		
		mq = mq_open(QUEUE_NAME, O_RDONLY);
		mq_receive(mq, &secret, sizeof(secret), NULL);
		
		fprintf(stdout,"child:  secret=%8d\n",secret);
	}else{
		fprintf(stderr,"Error: failed to fork with code: %d\n",p);
	}
	return 0;
}
