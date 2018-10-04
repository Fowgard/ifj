#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "symtable.h"


#define FILE_ERROR 5


int main(int argc, char *argv[])
{
	FILE *source_file;

	if (argc != 2)  // Pocet argumentu musi byt 2 - jmeno programu a nazev zdrojoveho souboru k prekladu 
	{
		fprintf(stderr, "Nesparvny pocet argumentu.\n");
		exit(FILE_ERROR);
	}
	
	if ((source_file = fopen(argv[1], "r")) == NULL) // Pokud se nepodari otevrit zdrojovy soubor k prekladu
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(FILE_ERROR);
	}

	
	set_source_file(source_file); //ve scanner.c

	//TODO inicializace tabulky symbolu

	//TODO inicializace seznamu instrukci
	//do token se nacitaji delsi lexemy jako nazvy promenych a podobne
	//jednomistne lexemy se vrati do result

	

	token_t *result;
	token_init(result);

	
	
	/*while (result->attribute[0] != END_OF_FILE)//TODO jeste doresit, nevieme jestli funguje, krajni situace
	{*/

		int haveLexem = 0;
		char *lexem = (char *) malloc(sizeof(char));
		lexem[0] = '\0';
		while(haveLexem != 1){
			addCharToLexem(&lexem);

			haveLexem = isLexem(&lexem);
		}
		printf("Lexém : %s\n", lexem);
		/*
		get_token(result);//ve scanner.c

		printf("%s\n",result->lexem);
		*/
		/*
		if (result.lexem == END_OF_LINE)
			printf(" KONEC RADKOVY\n");

		if (result.lexem == COMMENT)
			printf(" KOMMENT\n");

		if(result.lexem ==69)
			printf("TOKEN: %s \n", result.lexem);	
		//printf("TOKEN: %d \n", token[0]);
		*/

	/*}*/
	

	
	fclose(source_file);
	return 0;
}