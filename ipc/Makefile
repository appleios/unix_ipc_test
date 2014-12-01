all: test-fork	test-pipe test-fifo

test-fork: test-fork.c
	gcc -ansi -Wall -pedantic test-fork.c -o test-fork

test-pipe: test-pipe.c
	gcc -ansi -Wall -pedantic test-pipe.c -o test-pipe

test-fifo: test-fifo-writer.c test-fifo-reader.c
	gcc -ansi -Wall -pedantic test-fifo-writer.c -o test-fifo-writer
	gcc -ansi -Wall -pedantic test-fifo-reader.c -o test-fifo-reader	
