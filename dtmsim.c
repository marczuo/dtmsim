#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define DIRECTION int
#define LEFT -1
#define RIGHT 1
#define TSTART 5000
#define BLANK 'B'

struct Rule
{
	int forstate;
	char fortape;
	int tostate;
	char totape;
	DIRECTION direction;
};

int nrules;
struct Rule rules[1001];
char tape[10001];

int runTM(int head, int state)
{
	int i;
	
	for(i=1; i<=nrules; ++i)
	{
		if(rules[i].forstate != state) continue;
		if(rules[i].fortape != tape[head]) continue;
		tape[head] = rules[i].totape;
		return runTM(head + rules[i].direction, rules[i].tostate);
	}

	return state;
}

int main(int argc, char** argv)
{
	FILE* frule;
	int intbuf, i;
	char charbuf;
	int nstates, naccp, ntlen;
	int accp[10001];
	int finalstate;
	char buffer[1001];

	for(i=1; i<=10000; ++i) tape[i] = BLANK;

	if(argc != 3) return -1;
	frule = fopen(argv[1], "r");
	if(frule == NULL) return -1;

	fscanf(frule, "%d %d", &nstates, &naccp);
	for(i=1; i<=naccp; ++i) fscanf(frule, "%d", accp+i);
	
	fscanf(frule, "%d", &nrules);
	for(i=1; i<=nrules; ++i)
	{
		fscanf(frule, "%d", &rules[i].forstate);
		fscanf(frule, "%s", &buffer); rules[i].fortape = buffer[0];
		fscanf(frule, "%d", &rules[i].tostate);
		fscanf(frule, "%s", &buffer); rules[i].totape = buffer[0];
		fscanf(frule, "%s", &buffer); charbuf = buffer[0];
		if(charbuf == 'L') rules[i].direction = LEFT;
		else rules[i].direction = RIGHT;
	}

	for(i=0; i<strlen(argv[2]); ++i)
	{
		tape[TSTART+i] = argv[2][i];
	}

	finalstate = runTM(TSTART, 0);
	printf("Final tape (omitting blank squares):\n");
	for(i=1; i<=10000; ++i)
	{
		if(tape[i] != BLANK) printf("%c ", tape[i]);
	}
	printf("\n");
	for(i=1; i<=naccp; ++i) if(finalstate == accp[i]) { printf("Turing machine died accepting the input tape (%d).\n", finalstate); exit(0); }
	printf("Turing machine died NOT accepting the state (%d).\n", finalstate);

	return 0;
}
