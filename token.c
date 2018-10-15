#include "token.h"

 

void lexem_init(lexem_t *lexem)
{
	//lexem = malloc(sizeof (char *));
	if (lexem == NULL)
	{
		fprintf(stderr, "nepodarilo se alokovat lexem");
		exit(99);
	}
	lexem->word = malloc(sizeof(char));
	if (lexem->word == NULL)
	{
		fprintf(stderr, "nepodarilo se alokovat lexem->word");
		exit(99);
	}
	lexem->lenght = 0;
	lexem->word[0] = '\0';

}

void lexem_putchar(lexem_t *lexem, char symbol)
{
	lexem->word[lexem->lenght] = symbol;
	lexem->lenght++;
	lexem->word = realloc(lexem->word, (lexem->lenght+1)*sizeof(char));
	if (lexem->word == NULL)
	{
		fprintf(stderr, "nepodarilo se realokovat lexem");
		exit(99);
	}
	lexem->word[lexem->lenght+1] = '\0';
}



void token_init(token_t *token)
{

	//token->attribute = NULL; TODO implementovat hash table
	token->type = 0;
	token->attribute = NULL;

}

void set_type(token_t *token, int type)
{
	token->type = type;
}

int keyword_check(token_t *token, lexem_t *lexem)
{
	if (!strcmp(lexem->word, "def"))
		token->type = DEF;
	else if (!strcmp(lexem->word, "do"))
		token->type = DO;
	else if (!strcmp(lexem->word, "else"))
		token->type = ELSE;
	else if (!strcmp(lexem->word, "end"))
		token->type = END;
	else if (!strcmp(lexem->word, "if"))
		token->type = IF;
	else if (!strcmp(lexem->word, "not"))
		token->type = NOT;
	else if (!strcmp(lexem->word, "nil"))
		token->type = NIL;
	else if (!strcmp(lexem->word, "then"))
		token->type = THEN;
	else if (!strcmp(lexem->word, "while"))
		token->type = WHILE;
	else
		token->type = TYPE_IDENTIFIER;

	return 1;

}