all:
	gcc -o jpeg-bmp *.c

run: 
	./jpeg-bmp

zip:
	zip jpeg-bmp.zip *.c *.h Makefile