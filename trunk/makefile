# Generate executable files
dtmsim: dtmsim.o 
	gcc -g -o dtmsim dtmsim.o
dtmsim-release: dtmsim.o-release
	gcc -o RELEASE/dtmsim RELEASE/dtmsim.o
rulemkr: rulemkr.o
	gcc -g -o rulemkr rulemkr.o

# Cleanup
dtmsim-clean:
	rm -f dtmsim dtmsim.o
	rm -f RELEASE/dtmsim RELEASE/dtmsim.o
rulemkr-clean:
	rm -f rulemkr rulemkr.o

# Generate output files
dtmsim.o: dtmsim.c
	gcc -g -c dtmsim.c
dtmsim.o-release: dtmsim.c
	gcc -c -DNDEBUG dtmsim.c
	mkdir -p RELEASE
	mv dtmsim.o RELEASE/
rulemkr.o: rulemkr.c
	gcc -g -c rulemkr.c
