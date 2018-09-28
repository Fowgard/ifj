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

	
	set_source_file(source_file); //SCANNER

	//inicializace tabulky symbolu

	//inicializace seznamu instrukci

	char token[20] = "";//TODO implementovat STRING????
	//*token = "BRY DEN\n";

	//token[0] = 'a';
	//*token[1] = '\0';


	get_token(token);
	printf("TOKEN: %s \n", token);

	fclose(source_file);


	return 0;
}