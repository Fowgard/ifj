#include "scanner.h"



FILE *source_file;


void print_file()
{
	char c;

	c = fgetc(source_file); 
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
	char lexem[] = "";
	int state = 0;//vychozi stav automatu
	char symbol = INIT_SYMBOL; //Zde uchovavame vzdy dalsi nacteny znak z prekladaneho souboru
	while(state != END_OF_TOKEN)
	{
		symbol = fgetc(source_file);//nacteni jednoho symbolu(pismene)
		switch(state)
		{
			case STATE_START:	
				//Pokud program dosel na konec souboru
				if (symbol == EOF)
				{
					return END_OF_FILE;
				}
				//Ignorovat mezery na zacatku tokenu, btw tento if je zcela zbytečný
				if (isspace(symbol)){					
					state = STATE_START;
					break;
				}
				//Pokud je znak pismeno nebo cislo
				else if(isalnum(symbol))
				{
					strcat(lexem, &symbol); //Pridej znak k lexemu
					//Pokud je symbol pismeno, zmen stav na identifikator / klicove slovo
					if(isalpha(symbol)){
						state = STATE_ID_KW;
					}
					//Pokud je symbol cislo, zmen stav na cislo
					else if(isdigit(symbol)){
						state = STATE_INT;
					}
					//Pridat symbol k lexemu
					strcat(lexem, &symbol); //Pridej znak k lexemu		
				}
				//Pokud je znak komentar
				else if (symbol == '#')
					state = STATE_COMMENT;
			break;
			//nacitani identifikatoru nebo klicoveho slova
			case STATE_ID_KW:

				if (isspace(symbol))//mezera => byl nacten cely token
				{
					if(is_key_word(&symbol)){
						set_token(TYPE_KEYWORD, lexem);
					}else{
						set_token(TYPE_IDENTIFIER, lexem);
					}
					if (symbol == EOF)
					{
						return END_OF_FILE;
					}else
					{
						return SUCCESS;
					}
				}
				else
				{
					if(isalnum(symbol)){
						strcat(lexem, &symbol); //Pridej znak k lexemu						
					}else{
						//tady se osetri pripady kdy dalsi znak je napriklad +, / apod
					}
				}
			break;
			//nacitani cisla
			case STATE_INT:
				if (isspace(symbol))//mezera => byl nacten cely token
				{
					set_token(TYPE_INT, lexem);
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
							strcat(lexem, &symbol); //Pridej znak k lexemu	
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
					return COMMENT;
				}
					
			break;
		}		
	}
	return NEJAKA_KONSTANTA;
}

// Funkce ktera zkontroluje, zda neni lexem (predany v argumentu) klicove slovo
int is_key_word(char *lexem)
{
	return 0;
}
