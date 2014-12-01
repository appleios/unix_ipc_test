#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void signal_handler(int signum){
	printf("Got a signal!\n");
}

int main(int argc, char *argv[])
{
	signal(SIGUSR1,signal_handler);
	while (1) {
		sleep(1);
	}
	return 0;
}
