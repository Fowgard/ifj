#include <stdlib.h>
#include <stdio.h>

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


void get_token(char token[])
{
	token[0] = fgetc(source_file);



}



