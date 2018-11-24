#include "token.h"
#include "generator.h"

//makra pro jednodussi pirdavani instrukci/kodu
#define ADD_INST(inst)														\
	lexem_putstr(code, (inst "\n"))

#define ADD_CODE(code_to_add)														\
	lexem_putchar(code, (code_to_add))

lexem_t *code;
FILE *output_file;




void print_output()
{
	fputs(code->word, output_file);
	fclose(output_file);
}


void generator_init()
{
	printf("GENERATOR:\n");
	code = malloc(sizeof(lexem_t));
	if(code == NULL)
	{
		//funkce pro ukoncovani
	}
	lexem_init(code);	
	generate_header();
	
	if ((output_file = fopen("output.txt", "w")) == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(-1);
	}

	print_output();
	
}

void generate_header()
{
	ADD_INST(".IFJcode18");
	ADD_INST("JUMP $$main");

}



