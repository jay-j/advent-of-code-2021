COMMON=-O2 -std=c99 -mavx -pthread -Wall -pedantic -g

all: test

test: puzzle.bin
	./puzzle.bin test_input.txt
	./puzzle.bin test_input.txt > test_output.txt 
	diff test_correct.txt test_output.txt 

puzzle.bin: puzzle03.c 
	gcc $(COMMON) puzzle03.c -o puzzle.bin


go: puzzle.bin
	./puzzle.bin input.txt
