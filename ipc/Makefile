all: test-fork	test-pipe test-fifo test-signal test-shmem test-socket

test-fork: test-fork.c
	gcc -ansi -Wall -pedantic test-fork.c -o bin/test-fork

test-pipe: test-pipe.c
	gcc -ansi -Wall -pedantic test-pipe.c -o bin/test-pipe

test-fifo: test-fifo-writer.c test-fifo-reader.c
	gcc -ansi -Wall -pedantic test-fifo-writer.c -o bin/test-fifo-writer
	gcc -ansi -Wall -pedantic test-fifo-reader.c -o bin/test-fifo-reader	

test-signal: test-signal.c
	gcc -ansi -Wall -pedantic test-signal.c -o bin/test-signal

test-shmem: test-shmem.c
	gcc -ansi -Wall -pedantic test-shmem.c -o bin/test-shmem

test-socket: test-socket.c
	gcc -ansi -Wall -pedantic test-socket.c -o bin/test-socket

