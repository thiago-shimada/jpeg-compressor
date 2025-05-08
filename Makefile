all:
	gcc -std=c99 -o jpeg-bmp *.c -lm

run: 
	./jpeg-bmp

zip:
	zip jpeg-bmp.zip *.c *.h Makefile