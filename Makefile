.PHONY: all

all: compile

compile:
	gcc -c src/clibp.c -lstr -larr -lmap -lOS -g -g3 -ggdb -ggdb3
	sudo ar rcs clibp.a *.o; sudo mv clibp.a /usr/local/lib/libclibp.a; sudo rm *.o
	gcc clp.c -o clp -lclibp -lstr -larr -lmap -lOS -g -g3 -ggdb -ggdb3