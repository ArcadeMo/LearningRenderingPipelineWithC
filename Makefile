build:
	gcc -Wall -std=c99 ./src/*.c -lm -o renderer -I include -L lib -l SDL2-2.0.0

run: 
	./renderer

clean: 
	rm renderer