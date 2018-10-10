#include "token.h"

void token_init(token_t *token)
{
	token = malloc(sizeof(token_t));
	if (token == NULL)
		exit(99);
	key_counter++;
	token->key = key_counter;
	token->type = NULL;
	token->lexem = malloc(sizeof(char));
	if (token->lexem == NULL)
		exit(99);
	token->lexem[0] = '\0';
	token->lenght = 1;
}


void lexem_putchar(token_t *token, char *symbol, )
{
	token->lexem = realloc(token->lexem,(token->lenght + 1)*sizeof(char));
	if (token->lexem == NULL)
		exit(99);
	token->lexem[token->lenght-1] = *symbol;	//-1 protoze posledni symbol je '\0'
	token->lexem[token->lenght] = '\0';
	token->lenght++;
}

void set_type(token_t *token, int type)
{
	token->type = type;
}

int keyword_check(token_t *token)
{
	if (!strcmp(token->lexem, "def"))
		token->type = DEF;
	else if (!strcmp(token->lexem, "do"))
		token->type = DO;
	else if (!strcmp(token->lexem, "else"))
		token->type = ELSE;
	else if (!strcmp(token->lexem, "end"))
		token->type = END;
	else if (!strcmp(token->lexem, "if"))
		token->type = IF;
	else if (!strcmp(token->lexem, "not"))
		token->type = NOT;
	else if (!strcmp(token->lexem, "nil"))
		token->type = NIL;
	else if (!strcmp(token->lexem, "then"))
		token->type = THEN;
	else if (!strcmp(token->lexem, "while"))
		token->type = WHILE;
	else
		token->type = TYPE_IDENTIFIER;

}