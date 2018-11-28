#include "token.h" 
#include "generator.h"

//makra pro jednodussi pirdavani instrukci/kodu
#define CAT_INST(inst) \
	(gen_to_main == 1) ? lexem_putstr(code_main, (inst "\n")) : lexem_putstr(code_rest, (inst "\n"))


#define CAT_STR(code_to_add) \
	(gen_to_main == 1) ? lexem_putstr(code_main, (code_to_add)) : lexem_putstr(code_rest, (code_to_add))

#define CAT_NUM(num) \
	do{ \
		char buffer[310];	\
		sprintf(buffer, "%d", num);	\
		CAT_STR(buffer);\
	}while(0) \

#define CAT_FLOAT(num) \
	do{ \
		char buffer[310];	\
		sprintf(buffer, "%a", num);	\
		CAT_STR(buffer);\
	}while(0) \

#define CAT_ESC_SEQUENCE(symbol) \
	do{ \
		char buffer[310];	\
		sprintf(buffer, "%03d", symbol);	\
		CAT_STR(buffer);\
	}while(0) \

lexem_t *code_main;
lexem_t *code_rest;

FILE *output_file;
int gen_to_main;//zatim vse do main 
						//bude urcovat jestli generovat do mainu(jiny lexem) nebo do zbytku
int c_while = 0;

void gen_while_start(int condition)
{
	CAT_STR("LABEL START_W");
	CAT_NUM(c_while++);
	CAT_INST("");

	CAT_INST("JUMPIFEQ");
}

/*void gen_while_end()
{

}*/

void gen_frame_retvar()
{
	CAT_INST("CREATEFRAME");
	CAT_INST("PUSHFRAME");
	CAT_INST("DEFVAR LF@!retvar");// MUSI BYT NA LF PROTOZE PO KONCI FUNKCI SE DELA POPFRAME,
								// VEDLO BY KE ZTRATE HODNOTY

}

void gen_inputi()
{
	gen_frame_retvar();
	CAT_INST("READ LF@!retvar int");

	CAT_INST("POPFRAME");
	CAT_INST("RETURN");		
}

void gen_inputf()
{
	gen_frame_retvar();
	CAT_INST("READ LF@!retvar float");
	CAT_INST("POPFRAME");
	CAT_INST("RETURN");		
}
void gen_inputs()
{
	gen_frame_retvar();
	CAT_INST("READ LF@!retvar string");
	CAT_INST("POPFRAME");
	CAT_INST("RETURN");		
}
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
	gen_header();
	
	if ((output_file = fopen("output.txt", "w")) == NULL)
	{	
		fprintf(stderr, "Nepodarilo se otevrit soubor.\n");
		exit(-1);
	}

	//pomocna_docasna_funkce();

	
}

void pomocna_docasna_funkce()
{

	gen_main_start();
	gen_def_start("foo");
	gen_def_end("foo");
	print_output();
}

void gen_header()
{
	gen_to_main = 0;
	
	CAT_INST(".IFJcode18");
	CAT_INST("JUMP $$main");
	
}

void gen_main_start()
{
	gen_to_main = 1;
	CAT_INST(""); //prazdny radek
	CAT_INST("# main:");
	CAT_INST("LABEL $$main");
	CAT_INST("CREATEFRAME");
	CAT_INST("PUSHFRAME");
	CAT_INST("");
}
//mezi start a end se budou generovat ruzne dalsi instrukce
void gen_main_end()
{
	gen_to_main = 1;
	CAT_INST("");
	CAT_INST("POPFRAME");// je potreba?
	CAT_INST("CLEARS");
	CAT_INST("EXIT 0");//  je potreba?
	CAT_INST("");
}

void gen_var_from_token(token_t *token)
{
	if(token->type == TYPE_IDENTIFIER)
	{
		CAT_STR("LF@!");
		CAT_STR(token->attribute.string.word);
	}
	else if(token->type == TYPE_INT)
	{
		CAT_STR("int@");
		CAT_NUM(token->attribute.integer);
	}
	else if(token->type == TYPE_FLOAT)
	{
		CAT_STR("float@");
		CAT_FLOAT(token->attribute.decimal);
	}
	else if(token->type == TYPE_STRING)
	{
		CAT_STR("string@");
		int i = 0;
		char symbol = token->attribute.string.word[i]; 
		while(symbol != '\0')
		{
			if (symbol == '#' || symbol == '\\' || symbol < 33 || (!isprint(symbol)))
			{
				CAT_STR("\\");
				CAT_ESC_SEQUENCE(token->attribute.string.word[i]);
			}
			else
			{
				(gen_to_main == 1) ? lexem_putchar(code_main, symbol) : lexem_putchar(code_rest, symbol);
			}

			i++;
			symbol = token->attribute.string.word[i];
		}
	}
	else
	{
		//error

	}
}




void gen_def_start(char *f_name)
{
	gen_to_main = 0;

	CAT_INST("");
	CAT_STR("# zacatek funkce: "); 
	CAT_STR(f_name); 
	CAT_INST("");

	CAT_STR("LABEL $"); 
	CAT_STR(f_name);
	CAT_INST("");
	CAT_INST("PUSHFRAME");

}

void gen_def_return()//nemusi byt vzdy na konci = proto zvlast
{
	CAT_INST("MOVE LF@!retvar LF@!result");
	CAT_INST("POPFRAME");
	CAT_INST("RETURN");
}

void gen_def_end(char *f_name)
{
	//CAT_STR("LABEL $"); CAT_STR(function_id); CAT_STR("%return\n"); na co?
	CAT_INST("POPFRAME");
	CAT_INST("RETURN");
}
void create_frame()//musi se vyrvorit frame pro parametry, pro lepsi prehlednost pri volani z parseru
{
	CAT_INST("CREATEFRAME");
}

void gen_call(char *fname)
{
	CAT_STR("CALL $");
	CAT_STR(fname);
	CAT_INST("");
}

void gen_assign_from_call(char *v_name)
{
	CAT_STR("MOVE LF@!");
	CAT_STR(v_name);
	CAT_INST(" TF@!retvar");
}

