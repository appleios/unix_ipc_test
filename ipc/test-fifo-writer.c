#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int fd;
	int secret;
	char *FIFO_NAME;
	
	if(argc != 2){
		fprintf(stdout,"Usage: %s FIFO_NAME\n",argv[0]);
		return 1;
	}
	
	FIFO_NAME = argv[1];
	
	mkfifo(FIFO_NAME,0644);
	
	srand(time(0));
	secret = rand();
	fprintf(stdout,"parent: secret=%8d\n",secret);
	
	fd = open(FIFO_NAME,O_WRONLY);
	write(fd,&secret,sizeof(secret));
	close(fd);
	
	return 0;
}
