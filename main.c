#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "symtable.h"


#define FILE_ERROR 5

int main(int argc, char *argv[])
{


	FILE *source_file;

	/*
	if (argc != 2)
	{
		fprintf(stderr, "Nesparvny pocet argumentu.\n");
		exit(FILE_ERROR);
	}
	*/
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

	
	
	token_t *result = malloc(sizeof(token_t));
	if (result == NULL)
		exit(99);
	token_init(result);
	
	
	//zatim v komentu, predchystane na finalni verzi, volani parse

	/*
	int resul = parse;
	

	*/


	
	//zatim 1 v podmince, neni implementovana hash table pro ulozeni tokenu
	while (get_token(result) != END_OF_FILE)//result->word != END_OF_FILE //TODO jeste doresit, nevieme jestli funguje, krajni situace
	{
		printf(" %d\n",result->type);

		//ve scanner.c

		//printf("%s\n",result->attribute);

		/*
		if (result.lexem == END_OF_LINE)
			printf(" KONEC RADKOVY\n");

		if (result.lexem == COMMENT)
			printf(" KOMMENT\n");

		if(result.lexem ==69)
			printf("TOKEN: %s \n", result.lexem);	
		//printf("TOKEN: %d \n", token[0]);
		*/

	}


	
	
	fclose(source_file);
	return 0;
}