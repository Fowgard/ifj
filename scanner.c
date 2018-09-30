#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
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
	
	//print_file();	
}


int get_token(char token[])
{
	int state = 0;//vychozi stav automatu
	int symbol_order = 0;//kolikaty symbol tokenu je nacitan
	while(1)
	{
		char symbol = fgetc(source_file);//nacteni jednoho symbolu(pismene)
		switch(state)
		{
			case 0:	
				if (symbol == EOF)
					return END_OF_FILE;
				if (isspace(symbol))
					state = 0;//IGNORE
				//pismena nebo cislice
				else if(isalnum(symbol))
				{
					state = 1;
					token[symbol_order] = symbol;
					symbol_order++;
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
						return END_OF_FILE;
					return 69;//69 je docasne oznaceni konce tokenu, zmenit
				}
				else
				{
					token[symbol_order] = symbol;
					symbol_order++;
				}
			break;

			case 3:
				if (symbol == EOF)
					return END_OF_FILE;
				if (symbol == '\n')
					return COMMENT;
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



