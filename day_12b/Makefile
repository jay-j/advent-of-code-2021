COMMON=-O2 -std=c99 -mavx -pthread -Wall -pedantic -g

all: small

small: puzzle.bin
	./puzzle.bin small_input.txt
	./puzzle.bin small_input.txt > small_output.txt
	echo "Expect 36"
med: puzzle.bin
	./puzzle.bin med_input.txt
	echo "Expect 103"
big: puzzle.bin
	./puzzle.bin big_input.txt
	echo "Expect 3509"

puzzle.bin: puzzle.c 
	gcc $(COMMON) puzzle.c -o puzzle.bin


go: puzzle.bin
	./puzzle.bin input.txt
