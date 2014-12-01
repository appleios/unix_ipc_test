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
	struct stat st;
	char *FIFO_NAME;
	
	if(argc != 2){
		fprintf(stdout,"Usage: %s FIFO_NAME\n",argv[0]);
		return 1;
	}
	
	FIFO_NAME = argv[1];
	
	if( stat(FIFO_NAME,&st) == -1 && errno == ENOENT ){ /* no such file */
		if( mkfifo(FIFO_NAME,0644) == 0 && stat(FIFO_NAME,&st) == 0 ){
			if((st.st_mode & S_IFIFO) == S_IFIFO){
				fprintf(stderr,"Error: mkfifo created not a fifo\n");
				return 11;
			}
		}
	}else{
		if(st.st_mode != S_IFIFO){
			fprintf(stderr,"Error: file exists and is not a fifo\n");
			return 10;
		}
	}
	
	srand(time(0));
	secret = rand();
	fprintf(stdout,"parent: secret=%8d\n",secret);
	
	fd = open(FIFO_NAME,O_WRONLY);
	write(fd,&secret,sizeof(secret));
	close(fd);
	
	return 0;
}
