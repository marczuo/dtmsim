dtmsim: dtmsim.o 
	gcc -g -o dtmsim dtmsim.o
dtmsim.o: dtmsim.c
	gcc -g -c dtmsim.c
