#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "token.h"
#include "scanner.h"
#include "parser.h" 
#include "symtable.h"
#include "generator.h"
#include "stack.h"
#include "priority.h"



extern FILE* source_file;




int main(int argc, char *argv[])
{
	
	if (argc != 2)
	{
		fprintf(stderr, "Nesparvny pocet argumentu.\n");
		exit(99);
	}
	
	if ((source_file = fopen(argv[1], "r")) == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(99);
	}
	

	printf("ZACATEK parseru\n");
	int result =program();
	printf("KONEC parseru\n");
	free_all();
	fclose(source_file);

	return 0;
}