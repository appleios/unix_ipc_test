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
#define xDEBUG

typedef enum {
	TCP,
	UDP,
	NATIVE
} option;

int main(int argc, char *argv[])
{
	
	pid_t p;
	int i,t,sockid,cfd,domain,type,proto,secret;
	option op;
	unsigned ip = 0;
	unsigned port = 4545;
	char *file=".sock", *ipstring="127.0.0.1", *portstring="4545";
	int ipstring_len;
	struct sockaddr_un my_addr, peer_addr;
	struct sockaddr_in sock_addr;
	socklen_t peer_addr_size;
	struct in_addr buf_inet;
	
	if(argc<2 || argc>3){
		printf("Usage: %s [-t IP:PORT] [-u IP:PORT] [-n FILE]\n"
			   "OPTIONS:\n"
			   "-t\tfor TCP socket. Default IP:PORT are %s:%s.\n"
			   "-u\tfor UDP socket. Default IP:PORT are %s:%s.\n"
			   "-n\tfor Native. Default FILE is ``%s''.\n",argv[0],ipstring,portstring,ipstring,portstring,file);
		return 0;
	}
	
	if(argv[1][0]=='-' && (argv[1][1]=='t' || argv[1][1]=='u') && argv[1][2]==0){ /* tcp || udp */
		if (argv[1][1]=='t') {
			op = TCP;
			domain = PF_INET;
			type = SOCK_STREAM;
			proto = IPPROTO_IP;
		}else{
			op = UDP;
			domain = PF_INET;
			type = SOCK_DGRAM;
			proto = IPPROTO_IP;
		}
		
		if(argc==3){
			ipstring = strchr(argv[2],':');
			if (ipstring!=NULL) {
				ipstring_len = ipstring - argv[2];
				ipstring = (char*)malloc(ipstring_len + 1);
				strncpy(ipstring,argv[2],ipstring_len);
				ipstring[ipstring_len]=0;
				
				t = strlen(argv[2])-ipstring_len-1;
				portstring = (char*)malloc(t+1);
				for (i=0; i<t; i++) {
					portstring[i]=argv[2][ipstring_len+1+i];
				}
				portstring[i]=0;
			}else{
				ipstring = argv[2];
				ipstring_len = strlen(argv[2]);
			}
		}
		
#ifdef DEBUG
		printf("IP: %s\nPORT: %s\n",ipstring,portstring);
#endif

		t = inet_pton(AF_INET, ipstring, &buf_inet); /* or just inet_addr(ipstring) */
		if (t <= 0) {
			if (t == 0){
				fprintf(stderr, "Error: IP is not in presentation format: %s\n",ipstring);
				return 45;
			}
		}else{
			ip = buf_inet.s_addr;
		}
		port = atoi(portstring);
		
#ifdef DEBUG
		printf("IP: %u\nPORT: %d\n",ip,port);
#endif
		
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
	
	if(op == NATIVE){
		memset(&my_addr, 0, sizeof(struct sockaddr_un));
		my_addr.sun_family = AF_UNIX;
		strncpy(my_addr.sun_path, file,
				sizeof(my_addr.sun_path) - 1);
	}
	
	if(p>0){ /* parent */
		
		srand(time(0));
		secret = rand();
		fprintf(stdout,"parent: secret=%8d\n",secret);
		
		if (op == TCP || op == UDP) {
			sock_addr.sin_family = AF_INET;
			sock_addr.sin_addr.s_addr = ip;
			sock_addr.sin_port = htons(port);
			
			if( bind(sockid, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr_in)) < 0){
				close(sockid);
				fprintf(stderr,"Error: faild to bind socket. Details: %s.\n",strerror(errno));
				return 31;
			}
			
			if (listen(sockid, LISTEN_BACKLOG) == -1){
				close(sockid);
				fprintf(stderr,"Error: faild to listen socket port %d.\n",port);
				return 32;
			}
			
#ifdef DEBUG
			printf("Server is listening to socket.\n");
#endif
			
			if( (cfd = accept(sockid, NULL, NULL)) < 0){
				close(sockid);
				fprintf(stderr,"Error: faild to accept connection.\n");
				return 33;
			}
			
#ifdef DEBUG
			printf("Server is writing to socket\n");
#endif
			write(cfd,&secret,sizeof(secret));
			
#ifdef DEBUG
			printf("Server finished\n");
#endif
			
		}else if(op == NATIVE){
			if (bind(sockid, (struct sockaddr *) &my_addr,sizeof(struct sockaddr_un)) == -1){
				close(sockid);
				fprintf(stderr,"Error: faild to bind socket.\n");
				return 31;
			}
			
			if (listen(sockid, LISTEN_BACKLOG) == -1){
				close(sockid);
				fprintf(stderr,"Error: faild to listen socket.\n");
				return 32;
			}
			
			peer_addr_size = sizeof(struct sockaddr_un);
			cfd = accept(sockid, (struct sockaddr *) &peer_addr, &peer_addr_size);
			if (cfd < 0){
				close(sockid);
				fprintf(stderr,"Error: faild to accept connection.\n");
				return 33;
			}
			
			write(cfd,&secret,sizeof(secret));
		}
		
		wait(&t);
		close(sockid);
		if (op == NATIVE) {
			unlink(file);
		}
	}else if(p==0){ /* child */
		
#ifdef DEBUG
		printf("Child is connecting to socket\n");
#endif
		if (op == TCP || op == UDP) {
			sock_addr.sin_family = AF_INET;
			sock_addr.sin_addr.s_addr = ip;
			sock_addr.sin_port = htons(port);

			t = connect(sockid,(struct sockaddr *) &sock_addr,sizeof(struct sockaddr_in));
			if (t<0) {
				close(sockid);
				fprintf(stderr,"Errod: faild to establish connection. Details: %s.\n",strerror(errno));
				return 43;
			}
		}else{
			t = connect(sockid,(struct sockaddr *) &my_addr,sizeof(struct sockaddr_un));
			if (t<0) {
				close(sockid);
				fprintf(stderr,"Errod: faild to establish connection. Details: %s.\n",strerror(errno));
				return 43;
			}
		}

#ifdef DEBUG
		printf("Child is connected. Sockfd %d\n",sockid);
#endif

		read(sockid,&secret,sizeof(secret));
		fprintf(stdout,"child:  secret=%8d\n",secret);
		
#ifdef DEBUG
		printf("Child finished\n");
#endif
	}
	return 0;
}
