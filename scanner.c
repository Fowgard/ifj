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


void get_token(token_t *token)
{
	
	int state = 0;//vychozi stav automatu
	while(1)
	{
		char symbol = fgetc(source_file);//nacteni jednoho symbolu(pismene)
		switch(state)
		{
			case 0:	
				if (symbol == EOF)
				{
					token->lexem[0] = END_OF_FILE;
					return;
				}
					
				if (isspace(symbol))
					state = 0;//IGNORE
				//pismena nebo cislice
				else if(isalnum(symbol))
				{
					state = 1;
					token_putchar(symbol,token);
				}
				//komentar
				else if (symbol == '#')
					state = 3;
			break;
			//nacitani slova, cisla, atd
			case 1:

				if (isspace(symbol))//mezera => byl nacten cely token
				{
					if (symbol == EOF)
					{
						token->lexem[0] = END_OF_FILE;
						return;
					}
					return; //token;//69 je docasne oznaceni konce tokenu, zmenit
				}
				else
				{
					token_putchar(symbol,token);
				}
			break;

			case 3: //case komentar
				printf("%c",symbol);
				if (symbol == EOF)
				{
					token->lexem[0] = END_OF_FILE;
					return;
				}
					
				if (symbol == '\n')
				{
					token->lexem[0] = COMMENT;
					return;
				}
					
			break;
		}	
		
		/*
		if (isspace(symbol))//funkce z knihovny ctype.h
		{
			//mezery ingorovat, ale muze byt konec radku(v ruby dulezite, nejsou zavorky)
			
			
		}	
		else
		{
			//EOF
			if (symbol == EOF)
				return END_OF_FILE;
			//KOMENT
			else if (symbol == '#')
				state = 3;
			else if (isalnum(symbol))
				state = 1;

		}
		*/
		
		


		
	}

	//token[0] = fgetc(source_file); 
}



