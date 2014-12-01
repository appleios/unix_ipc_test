#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#define LISTEN_BACKLOG 10

typedef enum {
	TCP,
	UDP,
	NATIVE
} option;

int main(int argc, char *argv[])
{
	
	pid_t p;
	int t,sockid,cfd,domain,type,proto,secret;
	option op;
	unsigned long ip;
	unsigned port;
	char *file;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;
	
	if(argc!=3){
		printf("Usage: %s [-t IP:PORT] [-u IP:PORT] [-n FILE]\n"
			   "OPTIONS:\n"
			   "-t\tTCP socket\n"
			   "-u\tUDP socket\n"
			   "-n\tNative\n",argv[0]);
		return 0;
	}
	
	if(argv[1][0]=='-' && argv[1][1]=='t' && argv[1][2]==0){       /* tcp */
		op = TCP;
		domain = PF_INET;
		type = SOCK_STREAM;
		proto = IPPROTO_IP;
		sscanf(argv[2],"%lu:%u",&ip,&port);
	}else if(argv[1][0]=='-' && argv[1][1]=='u' && argv[1][2]==0){ /* udp */
		op = UDP;
		domain = PF_INET;
		type = SOCK_DGRAM;
		proto = IPPROTO_IP;
		sscanf(argv[2],"%lu:%u",&ip,&port);
	}else if(argv[1][0]=='-' && argv[1][1]=='n' && argv[1][2]==0){ /* native */
		op = NATIVE;
		domain = PF_LOCAL;
		type = SOCK_STREAM;
		proto = 0;
		file = strdup(argv[2]);
	}else{
		fprintf(stderr,"Error: unknown option: %s.\n",argv[1]);
		return 1;
	}
	
	p = fork();
	if(p<0){
		fprintf(stderr,"Error: failed to fork with code: %d\n",p);
		return 1;
	}
	
	sockid = socket(domain, type, proto);
	if (sockid<0) {
		close(sockid);
		fprintf(stderr,"Error: socket failed. Details: %s.\n",strerror(errno));
		return 30;
	}
	
	if (op == TCP) {
		
	}else if(op == UDP){
	}else if(op == NATIVE){
		memset(&my_addr, 0, sizeof(struct sockaddr_un));
		my_addr.sun_family = AF_UNIX;
		strncpy(my_addr.sun_path, file,
				sizeof(my_addr.sun_path) - 1);
	}
	
	if(p>0){
		
		if (bind(sockid, (struct sockaddr *) &my_addr,sizeof(struct sockaddr_un)) == -1){
			close(sockid);
			fprintf(stderr,"Errod: faild to bind socket.\n");
			return 31;
		}
		
		if (listen(sockid, LISTEN_BACKLOG) == -1){
			close(sockid);
			fprintf(stderr,"Errod: faild to listen socket.\n");
			return 32;
		}
		
		srand(time(0));
		secret = rand();
		fprintf(stdout,"parent: secret=%8d\n",secret);
		
		peer_addr_size = sizeof(struct sockaddr_un);
		cfd = accept(sockid, (struct sockaddr *) &peer_addr, &peer_addr_size);
		if (cfd == -1){
			close(sockid);
			fprintf(stderr,"Errod: faild to accept connection.\n");
			return 33;
		}
		
		if (op == TCP) {
			
		}else if(op == UDP){
		}else if(op == NATIVE){
			write(cfd,&secret,sizeof(secret));
		}
		
		wait(&t);
		if (op == NATIVE) {
			unlink(file);
		}
	}else if(p==0){
		t = connect(sockid,(struct sockaddr *) &my_addr,sizeof(struct sockaddr_un));
		if (t<0) {
			close(sockid);
			fprintf(stderr,"Errod: faild to establish connection. Details: %s.\n",strerror(errno));
			return 43;
		}
		
		read(sockid,&secret,sizeof(secret));
		fprintf(stdout,"child:  secret=%8d\n",secret);
	}
	return 0;
}
