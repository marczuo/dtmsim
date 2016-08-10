/*
	RULE FILE GENERATOR FOR DTMSIM
	COPYRIGHT (C) 2009 DOMUS-MARCIANA ZUO

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void nofiledie(char* filename)
{
	printf("Error: Can't find specified file: %s.\n", filename);
	exit(1);
}

void badargdie()
{
	printf("Invalid argument. Please specify output file.\n");
	exit(1);
}

int main(int argc, char** argv)
{
	FILE* foutput;
	int i, nstates, naccp, intbuf, nrules;
	char strbuf[10001];
	char charbuf;
	char* filename = malloc(10000);

	if(argc != 2) badargdie();
	strcpy(filename, argv[1]);
	if(strstr(filename, ".rule") == NULL || strcmp(strstr(filename, ".rule"), ".rule"))
		filename = strcat(filename, ".rule");
	foutput = fopen(filename, "w");
	if(foutput == NULL) nofiledie(filename);

	while(1)
	{
		printf("How many states are in the Turing machine? ");
		scanf("%d", &nstates);
		if(nstates > 0) break;
		printf("Number of states has to be at least 1.\n");
	}

	while(1)
	{
		printf("How many accepting states are in the Turing machine? ");
		scanf("%d", &naccp);
		if(naccp >= 0) break;
		printf("Number of states has to be at least 0.\n");
	}

	fprintf(foutput, "%d %d", nstates, naccp);

	for(i=1; i<=naccp; ++i)
	{
		printf("Accepting state #%d: ", i);
		scanf("%d", &intbuf);
		if(intbuf < 0 || intbuf > nstates - 1)
		{
			printf("State has to be between 0 and %d.\n", nstates - 1);
			i--; continue;
		}
		fprintf(foutput, " %d", intbuf);
	}

	printf("How many rules are there? ");
	scanf("%d", &nrules);
	fprintf(foutput, "\n%d\n", nrules);

	for(i=1; i<=nrules; ++i)
	{
		printf("\nRule #%d:\n", i);
		while(1)
		{
			printf("\tFor state: "); scanf("%d", &intbuf);
			if(intbuf < 0 || intbuf > nstates - 1)
			{
				printf("\tState has to be between 0 and %d.\n", nstates - 1);
				continue;	
			}
			break;
		}
		fprintf(foutput, "%d", intbuf);
		printf("\tFor input symbol: "); scanf("%s", &strbuf); charbuf = strbuf[0];
		fprintf(foutput, " %c", charbuf);
		while(1)
		{
			printf("\tChange to state: "); scanf("%d", &intbuf);
			if(intbuf < 0 || intbuf > nstates - 1)
			{
				printf("\tState has to be between 0 and %d.\n", nstates - 1);
				continue;
			}
			break;
		}
		fprintf(foutput, " %d", intbuf);
		printf("\tChange the tape square to: "); scanf("%s", &strbuf); charbuf = strbuf[0];
		fprintf(foutput, " %c", charbuf);
		while(1)
		{
			printf("\tMove (L)eft or (R)ight: "); scanf("%s", &strbuf); charbuf = strbuf[0];
			if(charbuf == 'L' || charbuf == 'R')
			{
				fprintf(foutput, " %c", charbuf);
				break;
			}
			printf("\tPlease type L or R.\n");
		}
		fprintf(foutput, "\n");
	}

	printf("Successfully generated rulefile: %s\n", filename);
	free(filename);

	return 0;
}
