#################################################
# DETERMINISTIC TURING MACHINE SIMULATOR
# COPYRIGHT (C) 2009 ZEE ZUO
#################################################

# Global macros
CC=gcc
NDEBUG=NDEBUG

# All targets
all: dtmsim rulemkr
release: dtmsim-release rulemkr-release

# Generate executable files
dtmsim: dtmsim.o 
	$(CC) -g -o dtmsim dtmsim.o
dtmsim-release: dtmsim.o-release
	$(CC) -o RELEASE/dtmsim RELEASE/dtmsim.o
rulemkr: rulemkr.o
	$(CC) -g -o rulemkr rulemkr.o
rulemkr-release: rulemkr

# Remove intermediate files
clean-o:
	rm -f *.o

# Cleanup
clean: dtmsim-clean rulemkr-clean
dtmsim-clean:
	rm -f dtmsim dtmsim.o
	rm -f RELEASE/dtmsim RELEASE/dtmsim.o
rulemkr-clean:
	rm -f rulemkr rulemkr.o

# Generate output files
dtmsim.o: dtmsim.c
	$(CC) -g -c dtmsim.c
dtmsim.o-release: dtmsim.c
	$(CC) -c -D$(NDEBUG) dtmsim.c
	mkdir -p RELEASE
	mv dtmsim.o RELEASE/
rulemkr.o: rulemkr.c
	$(CC) -g -c rulemkr.c
