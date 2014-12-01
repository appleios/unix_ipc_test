#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define KEY 12345
#define SKEY 123456

int main(int argc, char *argv[])
{
	pid_t p;
	int *secret;
	int t;
	int shmid;
	int semid;
	union semun sarg;
	struct sembuf sbuf;
	
	shmid = shmget(KEY, sizeof(int), IPC_CREAT|0644);
	if (shmid<0) {
		fprintf(stderr,"Error: can not get a shared memory identifier\n");
		return 20;
	}
	
	secret = shmat(shmid, NULL, 0);
	if (!secret) {
		fprintf(stderr,"Error: can not attach to a shared memory with identifier %d\n",shmid);
		return 21;
	}
	
	semid = semget(SKEY, 1, IPC_CREAT | SEM_R | SEM_A);
	if(semid<0){
		fprintf(stderr,"Error: can not get a semaphore identifier. Details: %s\n",strerror(errno));
		return 22;
	}
	
	/* set sem value to 0 */
	sarg.val = 0;
	semctl(semid,0,SETVAL,sarg);
	
	p = fork();
	
	if(p>0){
		srand(time(0));
		*secret = rand();
		
		fprintf(stdout,"parent: secret=%8d\n",*secret);
		
		/* signal operation */
		sbuf.sem_num =  0;
		sbuf.sem_op  =  1;
		sbuf.sem_flg =  0;
		semop(semid,&sbuf,1);
		
		wait(&t);
		
		shmdt(secret);
		shmctl(shmid, IPC_RMID, 0);
		
		semctl(semid, 0, IPC_RMID);
	}else if(p==0){
		
		/* wait operation */
		sbuf.sem_num =  0;
		sbuf.sem_op  = -1;
		sbuf.sem_flg =  0;
		semop(semid,&sbuf,1);
		
		fprintf(stdout,"child:  secret=%8d\n",*secret);
	}else{
		fprintf(stderr,"Error: failed to fork with code: %d\n",p);
	}
	return 0;
}
