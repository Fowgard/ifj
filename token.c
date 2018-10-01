void token_init(token_t *token)
{
	token->lexem = malloc(sizeof(char));
	if(token->lexem == NULL)
	{
		fprintf(stderr,"nemohli jsme allocovat\n");
		exit(99);
	}
	token->lenght = 1; //ve skutecnosti je tam nic
	token->lexem[0] = '\0';
}

void token_putchar(char *znak, token_t *token)
{
	token->lexem = realloc(*token->lexem,(token->lenght + 1)*sizeof(char));
	if (token->lexem == NULL)
	{
		fprintf(stderr,"nemohli jsme allocovat\n");
		exit(99);
	}
	token->lexem[token->lenght-1] = znak;	//-1 protoze posledni znak je '\0'
	token->lexem[token->lenght] = '\0';
	token->lenght++;
	//Hello darkness my old friend I come to see you again.
	
}