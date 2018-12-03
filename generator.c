#include "generator.h"

//makra pro jednodussi pridavani instrukci/kodu
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
	(void)condition; // at prekladac nerve

	CAT_STR("LABEL START_W");
	CAT_NUM(c_while++);
	CAT_INST("");
	CAT_INST("JUMPIFEQ");
}

/*void gen_while_end()
{

}*/

	// i<0?
	CAT_INST("LT LF@!relation LF@!1 int@0");
	CAT_INST("JUMPIFEQ $substr_end LF@!relation bool@true");
	// i>lengt?
	CAT_INST("GT LF@!relation LF@!1 LF@!length");
	CAT_INST("JUMPIFEQ $substr_end LF@!relation bool@true");

	CAT_INST("LT LF@!relation LF@!2 int@0");
	CAT_INST("JUMPIFEQ $substr_end LF@!relation bool@true");
	CAT_INST("MOVE LF@!retvar string@");

	CAT_INST("EQ LF@!relation LF@!2 int@0");
	CAT_INST("JUMPIFEQ $substr_end LF@!relation bool@true");
	// n > length(s) - i
	CAT_INST("DEFVAR LF@!n");
	CAT_INST("MOVE LF@!n LF@!length");
	CAT_INST("SUB LF@!n LF@!n LF@!1");
	
	CAT_INST("GT LF@!relation LF@!2 LF@!n");
	CAT_INST("JUMPIFEQ $n_is_max LF@!relation bool@true");
	CAT_INST("JUMP $find_substr");
	CAT_INST("LABEL $n_is_max");
	CAT_INST("MOVE LF@!2 LF@!n");//nastaveni na maximalni velikost n
	
	CAT_INST("LABEL $find_substr");
	CAT_INST("DEFVAR LF@!i");
	CAT_INST("MOVE LF@!i LF@!1");
	CAT_INST("DEFVAR LF@!symbol");
	CAT_INST("DEFVAR LF@!cond");
	CAT_INST("LABEL $substr_loop");
	
	CAT_INST("GETCHAR LF@!symbol LF@!0 LF@!i");
	CAT_INST("CONCAT LF@!retvar LF@!retvar LF@!symbol");
	CAT_INST("ADD LF@!i LF@!i int@1");
	CAT_INST("SUB LF@!2 LF@!2 int@1");//celkova delka stringu se zkrati, musime od n odecist 1
	CAT_INST("GT LF@!cond LF@!2 int@0");
	CAT_INST("JUMPIFEQ $substr_loop LF@!cond bool@true");
	CAT_INST("LABEL $substr_end");
	CAT_INST("POPFRAME");
	CAT_INST("RETURN");

}	

void gen_ord()
{
	CAT_INST("LABEL $ord");
	gen_frame_retvar();
	CAT_INST("MOVE LF@!retvar nil@nil");
	CAT_INST("DEFVAR LF@!relation");
	// i<0?
	CAT_INST("LT LF@!relation LF@!1 int@0");
	CAT_INST("JUMPIFEQ $ord_end LF@!relation bool@true");
	CAT_INST("DEFVAR LF@!length");
	//volani funkce length
	CAT_INST("CREATEFRAME");
	CAT_INST("DEFVAR TF@!0");
	CAT_INST("MOVE TF@!0 LF@!0");
	CAT_INST("CALL $length");
	CAT_INST("MOVE LF@!length TF@!retvar");
	CAT_INST("SUB LF@!length LF@!length int@1")
	// i>length?
	CAT_INST("GT LF@!relation LF@!1 LF@!length");
	CAT_INST("JUMPIFEQ ord_end LF@!relation bool@true");
	CAT_INST("STRI2INT LF@!retval LF@!0 LF@!1");
	CAT_INST("LABEL $ord_end");
	gen_def_end();
}

void gen_chr()
{
	CAT_INST("LABEL $chr");
	gen_frame_retvar();
	CAT_INST("DEFVAR LF@!relation");
	CAT_INST("LT LF@!relation LF@!0 int@0");
	CAT_INST("JUMPIFEQ $chr_error LF@!relation bool@true");
	CAT_INST("GT LF@!relation LF@!0 int@255");
	CAT_INST("JUMPIFEQ $chr_error LF@!relation bool@true");
	CAT_INST("INT2CHAR LF@!retvar LF@!0");
	gen_def_end();
	CAT_INST("LABEL $chr_error");
	CAT_INST("CLEARS");
	CAT_INST("EXIT 58");
}

void gen_length()
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

	pomocna_docasna_funkce();

	
}

void pomocna_docasna_funkce()
{

//	gen_main_start();
//	gen_def_start("foo");
//	gen_def_end("foo");
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

void gen_val_from_token(token_t *token)
{
	if(token->type == TYPE_IDENTIFIER)
	{
		CAT_STR("LF@!");
		CAT_STR(token->attribute.string->word);
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
		char symbol = token->attribute.string->word[i]; 
		while(symbol != '\0')
		{
			if (symbol == '#' || symbol == '\\' || symbol < 33 || (!isprint(symbol)))
			{
				CAT_STR("\\");
				CAT_ESC_SEQUENCE(token->attribute.string->word[i]);
			}
			else
			{
				(gen_to_main == 1) ? lexem_putchar(code_main, symbol) : lexem_putchar(code_rest, symbol);
			}

			i++;
			symbol = token->attribute.string->word[i];
		}
	}
	else
	{
		//error

	}
}


void gen_var(char *v_name)
{
	CAT_STR("DEFVAR LF@!");
	CAT_STR(v_name);
	CAT_INST("");
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
	(void)f_name; // at prekladac nerve

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

void gen_argument(token_t *arg, int order)
{
	CAT_STR("DEFVAR TF@!");
	CAT_NUM(order);
	CAT_INST("");

	CAT_STR("MOVE TF@!");
	CAT_NUM(order);
	gen_val_from_token(arg);
	CAT_INST("");	
}

void gen_parametr(char *p_name, int order)
{
	CAT_STR("DEFVAR LF@!");
	CAT_STR(p_name);
	CAT_INST("");

	CAT_STR("MOVE LF@!");
	CAT_STR(p_name);
	CAT_STR(" LF@!");
	CAT_NUM(order);
	CAT_INST("");
}