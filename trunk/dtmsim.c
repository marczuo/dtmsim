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

#define BOOL int
#define TRUE 1
#define FALSE 0

#define TSTART 5000
#define BLANK '_' /* L. Garron's suggestion */

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
	/* WARNING: PLEASE FREE ALL TAPE SQUARES!!!! */
int relhead;

/* Global Variables [glb] */
BOOL sbys;
#ifndef NDEBUG
BOOL debug;
#endif
FILE* foutput;
BOOL writeitout;

void relmem(struct ListNode* node)
{
#ifndef NDEBUG
	if(debug) printf("[DEBUG] Memory released at location %d.\n", node);
#endif
	if(node->next != NULL) relmem(node->next);
	free(node);
}

void pressenter()
{
	/* char buffer[10001]; */
	printf("Press enter to continue.");
	while (getchar() != '\n'); /* Courtesy of M. Sanders */
	/* fgets(buffer, 10000, stdin); */
}

int runTM(int state)
{
	int i, j, thb, nh;
		/* thb - Value of the tape head before making this move.
		   nh  - New head location relative to the first used square. */
	struct ListNode* nodbuf, *iter, *nhptr;
		/* nodbuf - Temporary pointer
		   iter   - Iterator
		   nhptr  - Pointer to new tape square */
	for(i=1; i<=nrules; ++i)
	{
		if(rules[i].forstate != state) continue;
		if(rules[i].fortape != tapehead->value) continue;
		thb = tapehead->value;
		tapehead->value = rules[i].totape;
		nh = relhead + rules[i].direction;

		if(nh < 0) /* Oops, gone out left */
		{	   /* Expand the valid tape region and allocate
			      memory for new tape square */
			nodbuf = (struct ListNode*)malloc(sizeof(struct ListNode));
#ifndef NDEBUG
			if(debug) printf("[DEBUG] Memory allocated at location %d.\n", nodbuf);
#endif
			initNode(nodbuf);
			insertNodeBef(tapehead, nodbuf, BLANK); 
			nh = 0; /* Adjust relative location to first used square */
			tapestart = nodbuf;
		}

		if(nh > relhead && tapehead->next == NULL) /* Oops, gone out right */
		{
			nodbuf = (struct ListNode*)malloc(sizeof(struct ListNode));
#ifndef NDEBUG
			if(debug) printf("[DEBUG] Memory allocated at location %d.\n", nodbuf);
#endif 
			initNode(nodbuf);
			insertNode(tapehead, nodbuf, BLANK);
		}

		if(rules[i].direction == LEFT) nhptr = tapehead->prev;
		else nhptr = tapehead->next;
		
		if(sbys) /* Step by step mode */
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

		if(writeitout) /* Write to file */
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

void nofiledie(char* filename)
{
	printf("Cannot open specified file: %s.\n", filename);
	printf("Try dtmsim --help for more information.\n");
	relmem(tapestart); /* Release memory occupied by the tape */
	exit(1);
}

void badargdie()
{
	printf("Invalid arguments.\n");
	printf("Try dtmsim --help for more information.\n");
	relmem(tapestart); /* Release memory occupied by the tape */
	exit(1);
}

void printhelp()
{
	printf("Deterministic Turing Machine SIMulator - Copyright (C) 2009 Zee Zuo\n");
	printf("Licensed under GNU General Public License v3\n\n");
	printf("Usage: dtmsim [options] rulefile input_tape\n\n");
	printf("\tOptions:\n");
	printf("\t\t--help or -h: Display this help message.\n");
	printf("\t\t--step or -s: Step by step mode (Useful for instruction).\n");
	printf("\t\t--output or -o filename: Save output to a file.\n");
#ifndef NDEBUG
	printf("\t\t--debug or -d: Display debug message.\n");
#endif
	printf("\n");
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
	printf("Please see <http://dtmsim.googlecode.com/> for more information.\n");
	printf("Alternatively, you may try rulemkr, an intuitive tool for writing a rulefile.\n");
	relmem(tapestart);
	exit(1);
}

int eq(char* left, char* right)
{
	return !strcmp(left, right);
}

int main(int argc, char** argv)
{
	FILE* frule; /* Rule file */
	int intbuf, i;
	char charbuf;
	int nstates, naccp;
		/* nstates - Number of states
		   naccp   - Number of accepting states */
	int accp[10001]; /* Array of accepting states */
	BOOL accpmrk[10001]; /* Array marking known accepting state
			       (In order to prevent duplicates) */
	int finalstate; /* Final state of the Turing machine */
	char buffer[1001];
	char* filename, *inputtape, *outputfile = malloc(10001);
#ifndef NDEBUG
	if(debug) printf("[DEBUG] Memory allocated at location %d.\n", outputfile);
#endif
		/* filename  - Name of the rule file */
	BOOL gotfilename = FALSE, gotinputtape= FALSE;
	BOOL accepted = FALSE;
	struct ListNode* iter;

	writeitout = FALSE;
	sbys = FALSE;
#ifndef NDEBUG
	debug = FALSE;
#endif

	/* Allocate first tape square */
	tapestart = malloc(sizeof(struct ListNode));
#ifndef NDEBUG
	if(debug) printf("[DEBUG] Memory allocated at location %d.\n", tapestart);
#endif
	initNode(tapestart);
	tapehead = tapestart;

	/* Set accepting state markers to FALSE */
	memset(accpmrk, FALSE, sizeof(accpmrk));

	/* Process command line arguments */
	for(i=1; i<argc; ++i)
	{
		if(eq(argv[i], "-h") || eq(argv[i], "--help")) printhelp();
		else if(eq(argv[i], "-s") || eq(argv[i], "--help")) sbys = TRUE;
		else if(eq(argv[i], "-o") || eq(argv[i], "--output"))
		{
			if(i == argc-1) badargdie();
			strcpy(outputfile, argv[++i]);
			if(strstr(outputfile, ".tape") == NULL || !eq(strstr(outputfile, ".tape"), ".tape")) 
				outputfile = strcat(outputfile, ".tape");
			writeitout = TRUE;
		}
#ifndef NDEBUG
		else if(eq(argv[i], "-d") || eq(argv[i], "--debug")) debug = TRUE;
#endif
		
		else if(!gotfilename)
		{
			gotfilename = TRUE;
			filename = argv[i];
		}
		else if(!gotinputtape)
		{
			gotinputtape = TRUE;
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
		printf("Please see <http://dtmsim.googlecode.com/> for more information.\n");
		exit(1);
		relmem(tapestart);
	}
	if(naccp < 0)
	{
		printf("In input file %s:\n", filename);
		printf("\tThe number of accepting states %d must be at least 0.\n", naccp);
		printf("Please see <http://dtmsim.googlecode.com/> for more information.\n");
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
			printf("Please see <http://dtmsim.googlecode.com/> for more information.\n");
			exit(1);
			relmem(tapestart);
		}
		accpmrk[accp[i]] = TRUE;	
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
			printf("Please see <http://dtmsim.googlecode.com/> for more information.\n");
			exit(1);
			relmem(tapestart);
		}
	}

	/* Put input tape onto the tape */
	tapehead->value = inputtape[0];
	for(i=1; i<strlen(inputtape); ++i)
	{
		tapehead = insertNode(tapehead, (struct ListNode*)malloc(sizeof(struct ListNode)), inputtape[i]);
#ifndef NDEBUG
		if(debug) printf("[DEBUG] Memory allocated at location %d.\n", tapehead);
#endif
	}

	if(writeitout)
	{
		foutput = fopen(outputfile, "w");
		if(foutput == NULL) nofiledie(outputfile);
	}

	/* If step by step then print first step */
	if(sbys)
	{
		iter = tapestart;
		printf("Start in state 0 and tape symbol %c.\n", iter->value);
		if(iter != NULL)
		{
			printf("H\n");
		
			printf("%c ", iter->value);
		
			while(iter->next != NULL)
			{
				iter = iter->next;
				printf("%c ", iter->value);
			}
		}

		printf("\n");
		pressenter();
	}

	/* If write to output file then print initial state */
	if(writeitout)
	{
		iter = tapestart;
		fprintf(foutput, "%d: ", 0);

		if(iter != NULL)
		{
			fprintf(foutput, "[%c]", iter->value);
			
			while(iter->next != NULL)
			{
				iter = iter->next;
				fprintf(foutput, "%c", iter->value);
			}

			fprintf(foutput, "\n");
		}
	}

	/* Run the Turing machine */
	tapehead = tapestart;
	relhead = 0;
	finalstate = runTM(0);

	/* Print final tape state */
	printf("Final tape (omitting blank squares):\n");
	iter = tapestart;
	if(iter != NULL)
	{
		if(iter->value != BLANK) printf("%c ", iter->value);

		while(iter->next != NULL)
		{
			iter = iter->next;
			if(iter->value != BLANK) printf("%c ", iter->value);
		}
	}
	printf("\n");

	/* Determine acceptance */
	for(i=1; i<=naccp; ++i) 
		if(finalstate == accp[i])
		{
			printf("Turing machine halted accepting the input tape (%d).\n", finalstate); 
			if(writeitout) fprintf(foutput, "ACCEPTED\n");
			accepted = TRUE;
			break;
		}

	if(!accepted)
	{
		printf("Turing machine halted NOT accepting the input tape (%d).\n", finalstate);
		if(writeitout) fprintf(foutput, "NOT ACCEPTED\n");
	}

	/* Clean up */
	relmem(tapestart);
#ifndef NDEBUG
	if(debug) printf("[DEBUG] Memory released at location %d.\n", outputfile);
#endif
	free(outputfile);
	fclose(frule);
	if(writeitout) fclose(foutput);

	return 0;
}
