#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	pid_t p = fork();
	int t;
	if(p>0){
		fprintf(stdout,"parent: pid %8u, child-pid  %8u\n",getpid(),p);
		wait(&t);
	}else if(p==0){
		fprintf(stdout,"child:  pid %8u, parent-pid %8u\n",getpid(),getppid());
	}else{
		fprintf(stderr,"Error: failed to fork with code: %d\n",p);
	}
	return 0;
}
