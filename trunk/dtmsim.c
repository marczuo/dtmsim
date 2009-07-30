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
#include "lnkdlst.h"
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
struct ListNode* tapestart, *tapehead;
int relhead;

/* Global Variables [glb] */
int sbys;
FILE* foutput;
int writeitout;

void relmem(struct ListNode* node)
{
	if(node->next != NULL) relmem(node->next);
	free(node);
}

void pressenter()
{
	//char buffer[10001];
	printf("Press enter to continue.");
	while (getchar() != '\n')
		;
//	fgets(buffer, 10000, stdin);
}

int runTM(int state)
{
	int i, j, thb, nh;
	struct ListNode* nodbuf, *iter, *nhptr;
	
	for(i=1; i<=nrules; ++i)
	{
		if(rules[i].forstate != state) continue;
		if(rules[i].fortape != tapehead->value) continue;
		thb = tapehead->value;
		tapehead->value = rules[i].totape;
		nh = relhead + rules[i].direction;

		if(nh < 0) /* Oops, gone out left */
		{
			nodbuf = (struct ListNode*)malloc(sizeof(struct ListNode));
			initNode(nodbuf);
			insertNodeBef(tapehead, nodbuf, BLANK); 
			nh = 0;
			tapestart = nodbuf;
		}

		if(nh > relhead && tapehead->next == NULL) /* Oops, gone out right */
		{
			nodbuf = (struct ListNode*)malloc(sizeof(struct ListNode));
			initNode(nodbuf);
			insertNode(tapehead, nodbuf, BLANK);
		}

		if(rules[i].direction == LEFT) nhptr = tapehead->prev;
		else nhptr = tapehead->next;
		
		if(sbys)
		{
			printf("\nFor state %d and tape symbol %c, write %c, go to state %d, and move ",
				state, thb, rules[i].totape, rules[i].tostate);
			if(nh > relhead) printf("right"); else printf("left"); printf(".\n");
			for(j=1; j<=nh*2; ++j) printf(" ");
			printf("H\n");
			
			iter = tapestart;
			printf("%c ", iter->value);

			while(iter->next != NULL)
			{
				iter = iter->next;
				printf("%c ", iter->value);
			}

			printf("\n");
			pressenter();	
		}

		if(writeitout)
		{
			fprintf(foutput, "%d: ", rules[i].tostate);

			iter = tapestart; j=0;
			if(nh == 0) fprintf(foutput, "[%c]", iter->value);
			else fprintf(foutput, "%c", iter->value);

			while(iter->next != NULL)
			{
				++j;
				iter = iter->next;
				if(j == nh) fprintf(foutput, "[%c]", iter->value);
				else fprintf(foutput, "%c", iter->value);
			}

			fprintf(foutput, "\n");
		}

		tapehead = nhptr;
		relhead = nh;
		return runTM(rules[i].tostate);
	}

	return state;
}

void unknowndie()
{
	printf("Unknown command: Type -h for help.\n");
	exit(1);
	relmem(tapestart);
}

void nofiledie(char* filename)
{
	printf("Cannot open specified file: %s.\n", filename);
	exit(1);
	relmem(tapestart);
}

void badargdie()
{
	printf("Invalid arguments. Type dtmsim -h for help.\n");
	exit(1);
	relmem(tapestart);
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
	printf("WARNING: IF YOU UPDATED FROM AN EARLY VERSION, MAKE SURE THAT YOU UPDATE THE RULEFILES AS WELL;\n");
	printf("YOU MAY ALSO UPDATE THE RULEFILES YOURSELF BY SUBSTITUTING \'_\' (UNDERSCORE) FOR ALL \'B\'s.\n");
	exit(0);
}

void invalidstatedie(char* filename, int nthrule, int wrongstate, int correctmaxstate)
{
	printf("In input file %s: \n", filename);
	printf("\tRule #%d: %d is not a valid state; should be between 0 and %d.\n", nthrule, wrongstate, correctmaxstate);
	exit(1);
	relmem(tapestart);
}

int eq(char* left, char* right)
{
	return !strcmp(left, right);
}

/* New function zone [nfz] */

int main(int argc, char** argv)
{
	FILE* frule;
	int intbuf, i;
	char charbuf;
	int nstates, naccp;
	int accp[10001];
	int accpmrk[10001];
	int finalstate;
	char buffer[1001];
	char* filename, *inputtape, *outputfile = malloc(10001);
	int gotfilename = 0, gotinputtape= 0;
	/* New Variable Zone [nvz] */
	writeitout = 0;

	tapestart = malloc(sizeof(struct ListNode));
	initNode(tapestart);
	tapehead = tapestart;

	memset(accpmrk, 0, sizeof(accpmrk));
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
			writeitout = 1;
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
	if(nstates < 1)
	{
		printf("In input file %s:\n", filename);
		printf("\tThe number of states %d must be at least 1.\n", nstates);
		exit(1);
		relmem(tapestart);
	}
	if(naccp < 0)
	{
		printf("In input file %s:\n", filename);
		printf("\tThe number of accepting states %d must be at least 0.\n", naccp);
		exit(1);
		relmem(tapestart);
	}

	for(i=1; i<=naccp; ++i)
	{
		fscanf(frule, "%d", accp+i);
		if(accpmrk[accp[i]])
		{
			printf("In input file %s:\n", filename);
			printf("\tAccepting state %d (%d) is a duplicate state.\n", i, accp[i]);
			exit(1);
			relmem(tapestart);
		}
		accpmrk[accp[i]] = 1;	
	}
	
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
			exit(1);
			relmem(tapestart);
		}
	}

	tapehead->value = inputtape[0];
	for(i=1; i<strlen(inputtape); ++i) tapehead = insertNode(tapehead, (struct ListNode*)malloc(sizeof(struct ListNode)), inputtape[i]);
	tapehead = tapestart;
	relhead = 0;

	if(writeitout)
	{
		foutput = fopen(outputfile, "w");
		if(foutput == NULL) nofiledie(outputfile);
	}

	if(sbys)
	{
		printf("Start in state 0 and tape symbol %c.\n", tapehead->value);
		if(tapehead != NULL)
		{
			printf("H\n");
		
			printf("%c ", tapehead->value);
		
			while(tapehead->next != NULL)
			{
				tapehead = tapehead->next;
				printf("%c ", tapehead->value);
			}
		}

		printf("\n");
		pressenter();
	}

	tapehead = tapestart;
	
	if(writeitout)
	{
		fprintf(foutput, "%d: ", 0);

		if(tapehead != NULL)
		{
			fprintf(foutput, "[%c]", tapehead->value);
			
			while(tapehead->next != NULL)
			{
				tapehead = tapehead->next;
				fprintf(foutput, "%c", tapehead->value);
			}

			fprintf(foutput, "\n");
		}
	}

	tapehead = tapestart;

	finalstate = runTM(0);
	printf("Final tape (omitting blank squares):\n");
	
	tapehead = tapestart;

	if(tapehead != NULL)
	{
		if(tapehead->value != BLANK) printf("%c ", tapehead->value);

		while(tapehead->next != NULL)
		{
			tapehead = tapehead->next;
			if(tapehead->value != BLANK) printf("%c ", tapehead->value);
		}
	}

	printf("\n");

	for(i=1; i<=naccp; ++i) 
		if(finalstate == accp[i])
		{
			printf("Turing machine halted accepting the input tape (%d).\n", finalstate); 
			if(writeitout) fprintf(foutput, "ACCEPTED\n");
		}
		else
		{
			printf("Turing machine halted NOT accepting the input tape (%d).\n", finalstate);
			if(writeitout) fprintf(foutput, "NOT ACCEPTED\n");
		}

	relmem(tapestart);
	free(outputfile);
	fclose(frule);
	if(writeitout) fclose(foutput);

	return 0;
}
