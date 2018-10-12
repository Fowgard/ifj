#include "token.h"



void lexem_init(lexem_t *lexem)
{
	lexem = malloc(sizeof (lexem_t));
	if (lexem == NULL)
	{
		fprintf(stderr, "nepodarilo se alokovat lexem");
		exit(99);
	}

	lexem->word[0] = '\0';

}

void lexem_putchar(lexem_t *lexem, char symbol)
{
	lexem->word[sizeof(lexem->word)-1] = symbol;
	lexem->word = realloc(lexem->word, sizeof(lexem->word) + 1);
	if (lexem->word == NULL)
	{
		fprintf(stderr, "nepodarilo se realokovat lexem");
		exit(99);
	}
	lexem->word[sizeof(lexem->word)-1] = '\0';
}



void token_init(token_t *token)
{

	//token->attribute = NULL; TODO implementovat hash table
	token->type = 0;


	/*
	token = malloc(sizeof(token_t));
	if (token == NULL)
		exit(99);
	key_counter++;
	*token->key = key_counter;
	token->type = NULL;
	token->lexem = malloc(sizeof(char));
	if (token->lexem == NULL)
		exit(99);
	token->lexem[0] = '\0';
	token->lenght = 1;
	*/
}

/*
void lexem_putchar(token_t *token, char *symbol)
{
	token->lexem = realloc(token->lexem,(token->lenght + 1)*sizeof(char));
	if (token->lexem == NULL)
		exit(99);
	token->lexem[token->lenght-1] = *symbol;	//-1 protoze posledni symbol je '\0'
	token->lexem[token->lenght] = '\0';
	token->lenght++;
}
*/
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