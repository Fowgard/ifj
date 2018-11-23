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
	char hex_value[2];
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
					state = STATE_STRING_LITERAL;
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
					printf("%s \n", lexem->word);
					

					return SUCCESS;
				}
			break;


			case STATE_NUMBER:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
				}
				else if (symbol == '.'  )
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_DOT;
				}
				else if(symbol == 'e' || symbol == 'E')
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_E;
				}
				else
				{
					char *ptr;
					ungetc(symbol, source_file);
					token->type = TYPE_INT;
					token->attribute.integer = strtol(lexem->word, &ptr, 10);
					
					printf("CISLO: %d\n",token->attribute.integer);
					return SUCCESS;
				}


			break;

			case STATE_DECIMAL_DOT:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL;
				}
				else
				{
					//ukonceni
					exit(0);
				}
			break;

			case STATE_DECIMAL:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
				}
				else if(symbol == 'e' || symbol == 'E')
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_E;
				}
				else
				{
					ungetc(symbol, source_file);
					token->type = TYPE_FLOAT;
					
					char *ptr;
					token->attribute.decimal = strtof(lexem->word, &ptr );
					
					printf("DESETINNE CISLO: %f\n",token->attribute.decimal);
					return SUCCESS;

				}
				
					
			break;

			case STATE_DECIMAL_E:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_END;
				}
				else if(symbol == '+' || symbol == '-')
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_E_SIGN;
				}
				else
				{
					//ukoncit
					exit(0);
				}
			break;

			case STATE_DECIMAL_E_SIGN:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
					state = STATE_DECIMAL_END;
				}
				else
				{
					//ukoncit
					exit(0);
				}


			break;

			case STATE_DECIMAL_END:
				if(isdigit(symbol))
				{
					lexem_putchar(lexem, symbol);
				}
				else
				{
					ungetc(symbol, source_file);
					token->type = TYPE_FLOAT;
					
					char *ptr;
					token->attribute.decimal = strtof(lexem->word, &ptr );
					
					printf("DESETINNE CISLO: %f\n",token->attribute.decimal);
					return SUCCESS;
				}
			break;

			case STATE_STRING_LITERAL:
				if(symbol == '\"')
				{
					token->type = TYPE_STRING;
					token->attribute.string.word = lexem->word;
					printf("LITERAL: %s\n", token->attribute.string.word);
					return SUCCESS;
				}
				else if(symbol == '\\')
				{
					state = STATE_ESCAPE_SEQUENCE;
				}
				else if(symbol < 32)
				{
					//ukoncit
					exit(0);
				}
				else
				{
					lexem_putchar(lexem, symbol);
				}


			break;

			case STATE_ESCAPE_SEQUENCE:
				if(symbol == '\"')
				{
					symbol = '\"';
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}					
				else if(symbol == 'n')
				{
					symbol = '\n';
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}
				else if(symbol == 't')
				{
					symbol = '\t';
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}
				else if(symbol == 's')
				{
					symbol = ' ';
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}
				else if(symbol == '\\')
				{
					symbol = '\\';
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}	
				else if(symbol == 'x')
				{
					state = STATE_HEXADECIMAL;
				}
				else
				{
					//ukoncit
					exit(0);
				}


			break;

			case STATE_HEXADECIMAL:

				//48 az 57 0-9, 97 az 102 a-f, 65 az 70 A-F
				if((symbol > 47 && symbol < 58) ||(symbol > 96 && symbol < 103)||(symbol > 64 && symbol < 71))
				{
					printf("SYM1: %c \n", symbol);
					hex_value[0] = symbol;
					state = STATE_HEXADECIMAL_SECOND;
				}
				else
				{
					//ukoncit
					exit(0);
				}

			break;

			case STATE_HEXADECIMAL_SECOND:
				//48 az 57 0-9, 97 az 102 a-f, 65 az 70 A-F
				if((symbol > 47 && symbol < 58) ||(symbol > 96 && symbol < 103)||(symbol > 64 && symbol < 71))
				{

					char *endptr;
					hex_value[1] = symbol;
					int value = (int) strtol(hex_value, &endptr, 16);
					symbol = (char) value;
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
				}
				else
				{
					char *endptr;
					ungetc(symbol, source_file);
					int value = (int) strtol(hex_value, &endptr, 16);
					symbol = (char) value;
					lexem_putchar(lexem, symbol);
					state = STATE_STRING_LITERAL;
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

