#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "symtable.h"


#define FILE_ERROR 5


int main(int argc, char *argv[])
{
	FILE *source_file;

	if (argc != 2) 
	{
		fprintf(stderr, "Nesparvny pocet argumentu.\n");
		exit(FILE_ERROR);
	}
	
	if ((source_file = fopen(argv[1], "r")) == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(FILE_ERROR);
	}

	
	set_source_file(source_file); //ve scanner.c

	//TODO inicializace tabulky symbolu

	//TODO inicializace seznamu instrukci
	//do token se nacitaji delsi lexemy jako nazvy promenych a podobne
	//jednomistne lexemy se vrati do result
	char token[20] = "";//TODO implementovat STRING???? 

	int result = 0;

	
	
	while (result != END_OF_FILE)
	{
		result = get_token(token);//ve scanner.c
		if (result == END_OF_LINE)
			printf(" KONEC RADKOVY\n");

		//printf("TOKEN: %d \n", token[0]);
	}
	

	
	fclose(source_file);
	return 0;
}