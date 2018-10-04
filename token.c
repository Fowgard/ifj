#include "token.h"
void token_init(token_t *token)
{
	token->type = malloc(sizeof(char)); // alokuje pro typ tokenu misto pro jeden char
	token->attribute = malloc(sizeof(char)); // alokuje pro atribut tokenu misto pro jeden char
	if((token->type == NULL)||(token->attribute == NULL)) // Pokud pri alokaci dojde k chybe, vypise ji a ukonci program
	{
		fprintf(stderr,"nemohli jsme allocovat\n");
		exit(99);
	}
	//token->lenght = 1; Misto uchovavani delky (indexu) budeme pouzivat funkci sizeof(token->type) resp token->attribute
	token->type[0] = '\0';
	token->attribute[0] = '\0';
}

void token_putchar(char *znak, token_t *token)
{
	token->attribute = realloc(token->attribute,(token->lenght + 1)*sizeof(char));
	if (token->attribute == NULL)
	{
		fprintf(stderr,"nemohli jsme allocovat\n");
		exit(99);
	}
	token->attribute[token->lenght-1] = *znak;	//-1 protoze posledni znak je '\0'
	token->attribute[token->lenght] = '\0';
	token->lenght++;
	//Hello darkness my old friend I come to see you again.
	
}