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

	while(1)
	{
		char symbol = fgetc(source_file);
		
		

		if (isspace(symbol))//funkce z knihovny ctype.h
		{
			//mezery ingorovat, ale muze byt konec radku(v ruby dulezite, nejsou zavorky)
			if (symbol == '\n')
				return END_OF_LINE;
			
		}	
		else
		{
			if (symbol == EOF)
				return END_OF_FILE;
			
			
		}
		
		
		switch(state)
		{
			case 0:


			break;
		}	


		
	}

	//token[0] = fgetc(source_file); 
}



