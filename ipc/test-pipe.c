#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[])
{
	int fd[2];
	pid_t p;
	int t;
	
	pipe(fd);
	p = fork();
	if(p>0){
		int secret;

		srand(time(0));
		secret = rand();
		
		write(fd[1],&secret,sizeof(secret));
		
		fprintf(stdout,"parent: secret=%8d\n",secret);
		wait(&t);
	}else if(p==0){
		int s;
		read(fd[0],&s,sizeof(s));

		fprintf(stdout,"child:  secret=%8d\n",s);
	}else{
		fprintf(stderr,"Error: failed to fork with code: %d\n",p);
	}
	return 0;
}
