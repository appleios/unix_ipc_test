all: test-fork	test-pipe test-fifo

test-fork: test-fork.c
	gcc -ansi -Wall -pedantic test-fork.c -o bin/test-fork

test-pipe: test-pipe.c
	gcc -ansi -Wall -pedantic test-pipe.c -o bin/test-pipe

test-fifo: test-fifo-writer.c test-fifo-reader.c
	gcc -ansi -Wall -pedantic test-fifo-writer.c -o bin/test-fifo-writer
	gcc -ansi -Wall -pedantic test-fifo-reader.c -o bin/test-fifo-reader	
