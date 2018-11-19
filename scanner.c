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
				//Pokud program dosel na konec souboru
				if (symbol == EOF)
				{
					set_type(token, END_OF_FILE);
					return END_OF_FILE;
				}
				
				else if(symbol == '\n')
				{
					set_type(token, END_OF_LINE);
					return END_OF_LINE;
				}
				else if(symbol == '\\')
				{
					state = STATE_BACKSLASH;//TODO(dodelat case)
				}
				else if(symbol == '+')
				{
					set_type(token, PLUS);
					return SUCCESS;
				}
				else if(symbol == '-')
				{
					set_type(token, MINUS);
					return SUCCESS;
				}
				else if(symbol == '*')
				{
					set_type(token, MUL);
					return SUCCESS;
				}
				else if(symbol == '/')
				{
					set_type(token, DIV);
					return SUCCESS;
				}
				else if(symbol == '=')
				{
					state = STATE_EQUALS;//TODO(dodelat case)
				}
				else if(symbol == '<')
				{
					state = STATE_LESSTHAN;//TODO(dodelat case)
				}
				else if(symbol == '>')
				{
					state = STATE_MORETHAN;//TODO(dodelat case)
				}
				



				

				else if (symbol == '#')
					state = STATE_COMMENT;

				else if(isspace(symbol))
					state = STATE_START;
				

				//Pokud je znak pismeno nebo cislo
				else if(isalnum(symbol))
				{
					//Pokud je symbol pismeno, zmen stav na identifikator / klicove slovo
					if(isalpha(symbol))
					{
						state = STATE_ID_KW;
						lexem_putchar(lexem, symbol);
					}
					//Pokud je symbol cislo, zmen stav na cislo
					else if(isdigit(symbol))
					{
						state = STATE_INT;
						set_type(token, TYPE_INT);//Pridat symbol k lexemu a nastavy typ					
						lexem_putchar(lexem, symbol);
					}
				}
				//Pokud je znak komentar
				
				//TODO "" uvozovky => literal(if (token->lexem[0] == "\"))
			break;
			//nacitani identifikatoru nebo klicoveho slova
			
			case STATE_ID_KW:

				if (isspace(symbol) || symbol == EOF)//mezera => byl nacten cely token
				{
					keyword_check(token, lexem); // musime resit jeste funkce T_T
					
					//podle typu dalsi operace

					printf("%s \n", lexem->word);
					return SUCCESS;
					
					
				}
				else
				{
					if(isalnum(symbol)){
						lexem_putchar(lexem, symbol);
					//Pridej znak k lexemu						
					}else{
						//tady se osetri pripady kdy dalsi znak je napriklad +, / apod
					}
				}
			break;
			//nacitani cisla
			case STATE_INT:
				if (isspace(symbol))//mezera => byl nacten cely token
				{
					//set_type(TYPE_INT, lexem);
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

