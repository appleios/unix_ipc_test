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
	
	fd = open(FIFO_NAME,O_RDONLY);
	read(fd,&secret,sizeof(secret));
	close(fd);

	fprintf(stdout,"child:  secret=%8d\n",secret);
	
	unlink(FIFO_NAME);
	
	return 0;
}
