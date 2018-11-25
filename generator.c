#include "token.h"
#include "generator.h"

//makra pro jednodussi pirdavani instrukci/kodu
#define ADD_INST(inst) \
	(generate_to_main == 1) ? lexem_putstr(code_main, (inst "\n")) : lexem_putstr(code_rest, (inst "\n"))


#define ADD_CODE(code_to_add) \
	(generate_to_main == 1) ? lexem_putstr(code_main, (code_to_add)) : lexem_putstr(code_rest, (code_to_add))



lexem_t *code_main;
lexem_t *code_rest;

FILE *output_file;
int generate_to_main;//zatim vse do main 
						//bude urcovat jestli generovat do mainu(jiny lexem) nebo do zbytku



void print_output()
{
	fputs(code_rest->word, output_file);
	fputs(code_main->word, output_file);	
	fclose(output_file);
}


void generator_init()
{
	code_rest = malloc(sizeof(lexem_t));
	if(code_rest == NULL)
	{
		//funkce pro ukoncovani
	}
	lexem_init(code_rest);	

	code_main = malloc(sizeof(lexem_t));
	if(code_main == NULL)
	{
		//funkce pro ukoncovani
	}
	lexem_init(code_main);	
	generate_header();
	
	if ((output_file = fopen("output.txt", "w")) == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(-1);
	}

	pomocna_docasna_funkce();

	
}

void pomocna_docasna_funkce()
{

	generate_main_start();
	generate_def_start("foo");
	generate_def_end("foo");
	print_output();
}

void generate_header()
{
	generate_to_main = 0;
	
	ADD_INST(".IFJcode18");
	ADD_INST("JUMP $$main");
	
}

void generate_main_start()
{
	generate_to_main = 1;
	ADD_INST(""); //prazdny radek
	ADD_INST("# main:");
	ADD_INST("LABEL $$main");
	ADD_INST("CREATEFRAME");
	ADD_INST("PUSHFRAME");
	ADD_INST("");
}
//mezi start a end se budou generovat ruzne dalsi instrukce
void generate_main_end()
{
	generate_to_main = 1;
	ADD_INST("");
	ADD_INST("POPFRAME");// je potreba?
	ADD_INST("CLEARS");
	ADD_INST("EXIT 0");//  je potreba?
	ADD_INST("");
}


void generate_def_start(char *f_name)
{
	generate_to_main = 0;

	ADD_INST("");
	ADD_CODE("# zacatek funkce: "); 
	ADD_CODE(f_name); 
	ADD_INST("");

	ADD_CODE("LABEL $"); 
	ADD_CODE(f_name);
	ADD_INST("");
	ADD_INST("PUSHFRAME");

}

void generate_def_end(char *f_name)
{
	//ADD_CODE("LABEL $"); ADD_CODE(function_id); ADD_CODE("%return\n"); na co?
	ADD_INST("POPFRAME");
	ADD_INST("RETURN");
}

