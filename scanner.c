#include "scanner.h"




FILE *source_file;

int BLOCK_COMMENT = FALSE;
 

void print_file()
{
	char c = fgetc(source_file); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(source_file); 

    } 
    printf("\n");
}


void set_source_file(FILE *f)
{
	source_file = f;
}

int get_token(token_t *token)
{
	lexem_t *lexem = malloc(sizeof (lexem_t));
	if(lexem == NULL)
	{
		//funkce pro ukoncovani
	}
	lexem_init(lexem);
	int state = STATE_START;//vychozi stav automatu
	char symbol; //Zde uchovavame vzdy dalsi nacteny znak z prekladaneho souboru
	while(state != END_OF_TOKEN)
	{
		symbol = fgetc(source_file);//nacteni jednoho symbolu(pismene)
		//blokovy komentar
		if (symbol == '\n')
		{
			char tmp_symbol = fgetc(source_file);
			if (tmp_symbol == '=')
			{
				int comment = 1;
				lexem_t *lexem_com = malloc(sizeof (lexem_t));
				
				if(lexem_com == NULL)
				{
					//funkce pro ukoncovani
				}
				
				lexem_init(lexem_com);
				while (!isspace(tmp_symbol) || tmp_symbol == EOF)
				{
					
					lexem_putchar(lexem_com, tmp_symbol);
					tmp_symbol = fgetc(source_file);
				}
				if(tmp_symbol == EOF)
				{
						//ukoncit
				}
				
				if (!strcmp(lexem_com->word,"=begin"))
				{
					lexem_del_word(lexem_com);
					while(comment)
					{
						while((tmp_symbol = fgetc(source_file)) != '\n' && tmp_symbol != EOF);
						

						if(tmp_symbol == EOF)
						{
						//SPRAVNE UKONCIT
							exit(0);
						}	

						tmp_symbol = fgetc(source_file);

						if (tmp_symbol == '=')
						{
							while (!isspace(tmp_symbol) || tmp_symbol == EOF)
							{
								lexem_putchar(lexem_com, tmp_symbol);
								tmp_symbol = fgetc(source_file);
							}
							if(tmp_symbol == EOF)
							{
							//SPRAVNE UKONCIT
								exit(0);
							}	
							if (!strcmp(lexem_com->word,"=end"))
							{
								comment = 0;
								while((tmp_symbol = fgetc(source_file)) != '\n');
							}
							else
							{
								//ukoncit
							}
						}
					}
				}
				else
				{
					fprintf(stderr, "ERROR: %s\n",lexem->word);
					//ukoncit
				}
			}
			else
				ungetc(tmp_symbol, source_file);
		}

		switch(state)
		{
			case STATE_START:	
				if (symbol == EOF)
				{
					token->type = END_OF_FILE;
					return END_OF_FILE;
				}
				
				else if(symbol == '\n')
				{
					token->type =END_OF_LINE;
					return END_OF_LINE;
				}
				else if(symbol == '+')
				{
					token->type = PLUS;
					return SUCCESS;
				}
				else if(symbol == '-')
				{
					token->type = MINUS;
					return SUCCESS;
				}
				else if(symbol == '*')
				{
					token->type = MUL;
					return SUCCESS;
				}
				else if(symbol == '/')
				{
					token->type = DIV;
					return SUCCESS;
				}
				else if(symbol == '=')
				{
					state = STATE_EQUALS;
				}
				else if(symbol == '<')
				{
					state = STATE_LESSTHAN;
				}
				else if(symbol == '>')
				{
					state = STATE_MORETHAN;
				}
				else if(symbol == '!')
				{
					state = STATE_EXCLAMATION;
				}
				else if(symbol == ',')
				{
					token->type = COMMA;
					return SUCCESS;
				}				
				else if(symbol == ')')
				{
					token->type = LEFT_BRACKET;
					return SUCCESS;
				}
				else if(symbol == '(')
				{
					token->type = RIGHT_BRACKET;
					return SUCCESS;
				}



				

				else if (symbol == '#')
					state = STATE_COMMENT;

				else if(isspace(symbol))
					state = STATE_START;
				
				

				else if (isalpha(symbol) || symbol == '_')//TODO co delat s velkymi pismeny?
				{
					state = STATE_ID_KW;
					lexem_putchar(lexem, symbol);
				}
				else if (isdigit(symbol))
				{
					state = STATE_NUMBER;
					lexem_putchar(lexem, symbol);
				}
				else if (symbol == '\"')
				{
					state = STATE_STRING_LITERAL;//TODO case
				}
			break;

			case STATE_EQUALS:
				if(symbol == '=')
				{
					token->type = COMPARE;
					return SUCCESS;
				}
				else 
				{
					token->type = EQUALS;
					ungetc(symbol, source_file);
					return SUCCESS;
				}
			break;

			case STATE_LESSTHAN:
				if(symbol == '=')
				{
					token->type = LOE;//less or equal
					return SUCCESS;
				}
				else
				{
					token->type = LESSTHAN;
					ungetc(symbol, source_file);
					return SUCCESS;
				}
			break;

			case STATE_MORETHAN:
				if(symbol == '=')
				{
					token->type = MOE;//more or equal
					return SUCCESS;
				}
				else 
				{
					token->type = MORETHAN;
					ungetc(symbol, source_file);
					return SUCCESS;
				}
			break;

			case STATE_EXCLAMATION:
				if(symbol == '=')
				{
					token->type = NOTEQUAL;
					return SUCCESS;
				}
				else
				{
					//spravne ukoncit
				}
			break;
			
			
			case STATE_ID_KW:

				if(isalnum(symbol) || symbol == '_')
				{
					lexem_putchar(lexem, symbol);
				}
				else
				{
					ungetc(symbol, source_file);

					keyword_check(token, lexem); // musime resit jeste funkce T_T
					
					token->attribute.string = *lexem;
					//podle typu dalsi operace
					printf("%s \n", lexem->word);
					

					return SUCCESS;
				}
			break;
			//nacitani cisla
			case STATE_NUMBER:
				if (isspace(symbol))//mezera => byl nacten cely token
				{
					if (symbol == EOF)
					{
						return END_OF_FILE;
					}else{
						return SUCCESS;
					}
				}
				else
				{
					if(isalnum(symbol)){
						//Pokud je symbol cislo, tak se prida ke stavajicimu lexemu
						if(isdigit(symbol))
						{
							 //Pridej znak k lexemu	
						}
						//Pokud je symbol alfanumericky znak a neni cislo, vrat chybu - cislo nemuze byt spojeno s pismeny
						else
						{
							return WRONG_NUMBER_FORMAT;
						}											
					}else{
						//tady se osetri pripady kdy dalsi znak je napriklad +, / apod
					}
				}
			break;
			// Pokud je symbol komentar
			case STATE_COMMENT: 
				if (symbol == EOF)
				{
					return END_OF_FILE;
				}
					
				if (symbol == '\n')
				{
					return SUCCESS;
				}
					
			break;
		}		
	}
	return KONSTANT;
}

