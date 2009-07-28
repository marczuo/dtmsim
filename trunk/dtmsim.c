/*
	DETERMINISTIC TURING MACHINE SIMULATOR
	COPYRIGHT (C) 2009 ZEE ZUO

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	ALONG WITH this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define DIRECTION int
#define LEFT -1
#define RIGHT 1
#define TSTART 5000
#define BLANK '_'

struct Rule
{
	int forstate;
	char fortape;
	int tostate;
	char totape;
	DIRECTION direction;
};

int nrules;
struct Rule rules[10001];
char tape[10001];

int sbys;
int leftmost, rightmost;

void pressenter()
{
	char buffer[10001];
	printf("Press enter to continue.");
	fgets(buffer, 10000, stdin);
}

int runTM(int head, int state)
{
	int i, j, thb, nh;
	
	for(i=1; i<=nrules; ++i)
	{
		if(rules[i].forstate != state) continue;
		if(rules[i].fortape != tape[head]) continue;
		thb = tape[head];
		tape[head] = rules[i].totape;
		nh = head + rules[i].direction;
		if(nh<leftmost) leftmost = nh;
		if(nh>rightmost) rightmost = nh;
		if(sbys)
		{
			printf("\nFor state %d and tape symbol %c, write %c, go to state %d, and move ",
				state, thb, rules[i].totape, rules[i].tostate);
			if(nh > head) printf("right"); else printf("left"); printf(".\n");
			if(nh != leftmost) for(j=1; j<=(nh-leftmost)*2; ++j) printf(" ");
			printf("H\n");
			for(j=leftmost; j<=rightmost; ++j) printf("%c ", tape[j]);
			printf("\n");
			pressenter();
			printf("\n");
		}

		return runTM(nh, rules[i].tostate);
	}

	return state;
}

void unknowndie()
{
	printf("Unknown command: Type -h for help.\n");
	exit(1);
}

void nofiledie(char* filename)
{
	printf("Cannot find specified file: %s.\n", filename);
	exit(1);
}

void badargdie()
{
	printf("Invalid arguments. Type dtmsim -h for help.\n");
	exit(1);
}

void printhelp()
{
	printf("Deterministic Turing Machine SIMulator - Copyright (C) 2009 Zee Zuo\n");
	printf("Licensed under GNU General Public License v3\n\n");
	printf("Usage: dtmsim [options] rulefile input_tape\n\n");
	printf("\tOptions:\n");
	printf("\t\t-h: Display this help message.\n");
	printf("\t\t-s: Step by step mode (Useful for instruction).\n");
	printf("\t\t-o filename: Save output to a file.\n\n");
	printf("\trulefile: A file representing the simulated Turing machine. See <http://dtmsim.googlecode.com/> for more information.\n");
	printf("\tinput_tape: The input tape. Note that \'_\' (underscore) is used as the blank symbol.\n");
	printf("\nNote: If the input tape is intended to be blank, use a single character \'_\'.\n");
	exit(0);
}

void invalidstatedie(char* filename, int nthrule, int wrongstate, int correctmaxstate)
{
	printf("In input file %s: \n", filename);
	printf("\tRule #%d: %d is not a valid state; should be between 0 and %d.\n", nthrule, wrongstate, correctmaxstate);
	exit(1);
}

int eq(char* left, char* right)
{
	return !strcmp(left, right);
}

int main(int argc, char** argv)
{
	FILE* frule;
	int intbuf, i;
	char charbuf;
	int nstates, naccp;
	int accp[10001];
	int finalstate;
	char buffer[1001];
	char* filename, *inputtape, *outputfile = malloc(10001);
	FILE* foutput;
	int writeoutit = 0, gotfilename = 0, gotinputtape= 0;

	for(i=1; i<=10000; ++i) tape[i] = BLANK;
	sbys = 0;
		
	for(i=1; i<argc; ++i)
	{
		if(eq(argv[i], "-h") || eq(argv[i], "--help")) printhelp();
		else if(eq(argv[i], "-s")) sbys = 1;
		else if(eq(argv[i], "-o"))
		{
			if(i == argc-1) badargdie();
			strcpy(outputfile, argv[++i]);
			if(strstr(outputfile, ".tape") == NULL || !eq(strstr(outputfile, ".tape"), ".tape")) 
				outputfile = strcat(outputfile, ".tape");
			writeoutit = 1;
		}
		
		else if(!gotfilename)
		{
			gotfilename = 1;
			filename = argv[i];
		}
		else if(!gotinputtape)
		{
			gotinputtape = 1;
			inputtape = argv[i];
		}
		else badargdie();
	}

	if(!gotinputtape) badargdie();

	frule = fopen(filename, "r");
	if(frule == NULL) nofiledie(filename);
	
	fscanf(frule, "%d %d", &nstates, &naccp);
	for(i=1; i<=naccp; ++i) fscanf(frule, "%d", accp+i);
	
	fscanf(frule, "%d", &nrules);
	for(i=1; i<=nrules; ++i)
	{
		fscanf(frule, "%d", &rules[i].forstate);
		if(rules[i].forstate >= nstates || rules[i].forstate < 0) invalidstatedie(filename, i, rules[i].forstate, nstates-1);
		fscanf(frule, "%s", &buffer); rules[i].fortape = buffer[0];
		fscanf(frule, "%d", &rules[i].tostate);
		if(rules[i].tostate >= nstates || rules[i].tostate < 0) invalidstatedie(filename, i, rules[i].tostate, nstates-1);
		fscanf(frule, "%s", &buffer); rules[i].totape = buffer[0];
		fscanf(frule, "%s", &buffer); charbuf = buffer[0];
		if(charbuf == 'L') rules[i].direction = LEFT;
		else if(charbuf == 'R') rules[i].direction = RIGHT;
		else
		{
			printf("In input file %s:\n", filename);
			printf("\tRule #%d: %s is not a legal option; must be either L or R.\n", i, buffer);
		}
	}

	for(i=0; i<strlen(inputtape); ++i) tape[TSTART+i] = inputtape[i];
	
	leftmost = TSTART;
	rightmost = TSTART + strlen(inputtape) - 1;
	if(sbys)
	{
		printf("Start in state 0 and tape symbol %c.\n", tape[TSTART]);
		printf("H\n");
		for(i=leftmost; i<=rightmost; ++i) printf("%c ", tape[i]); printf("\n");
		pressenter();
	}

	finalstate = runTM(TSTART, 0);
	printf("Final tape (omitting blank squares):\n");
	for(i=leftmost; i<=rightmost; ++i) printf("%c ", tape[i]);
	printf("\n");
	for(i=1; i<=naccp; ++i) 
		if(finalstate == accp[i])
			printf("Turing machine halted accepting the input tape (%d).\n", finalstate); 
		else
			printf("Turing machine halted NOT accepting the input tape (%d).\n", finalstate);

	if(writeoutit)
	{
		foutput = fopen(outputfile, "w");

		for(i=leftmost; i<=rightmost; ++i)
			fprintf(foutput, "%c", tape[i]); 
		
		fprintf(foutput, "\n");
	}
		
	return 0;
}
