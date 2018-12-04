#include "parser.h"
  
extern int gen_to_main;
extern int while_counter;
extern int if_counter;
int token_type; //aktualni token
char *token_attribute; //atribut aktualniho tokenu
token_t *token = NULL;
token_t *tmp = NULL;//zjisteni dalsiho tokenu -> z toho budem vedet jestli se vola o volani nedefinovane funkce, nebo jestli se jedna o definici proměne
tStack stack; //Zasobnik pro vyrazy
Token_Stack values; //Zasobnik pro tokeny
tStack operatory; //Zasobnik pro hodnoty
tHTable* h_tabulka; // Hashovaci tabulka
int brackets_counter=0; //Leve zavorky ++, prave --
int param_counter; //pocitadlo parametru pri definici funkce
int porovnavani_counter;//pocitadlo >,<,<=,>=,==,!=
bool already_init=false;//podmínka pro inicializaci parseru
int uvnitr_funkce=0;//pro orientaci jestli jsme v definic funkce nebo ne 
int typ_promene=WITHOUT_TYPE;//typ promene
bool uzavorkovana_funkce=false; // pro volani funkce flag na poznani jestli je funkce uzavorkovana nebo ne 
int pocitac_param_u_call=0;//pocitadlo argumentu pro volani funkce
bool last_id_func;//jestli poslední identifikator byla funkce nebo promena
int volani_func = 0;//pocet vnoreni volani funkci
int last_KW=WITHOUT_TYPE;//posledni pouzite klicove slovo ( je to pouze pro kontrolu aby nebyli pod sebou 2x else)
int deep_in_program = 0;//pocitadlo pro hloubku programu ( kvuli while,if apod)
char *last_function;//nazev posledni pouzite funkce
int last_if_counter=0;//pocitadlo pro ify
bool skipni_get_token=false;//pro preskoceni nahrani dalsiho token (jedná se o to, že když máme identifier tak musíme načítat i token za tím, tak abychom vlastně jeden token nezahodil)
int remember_token_type;//slouží pro uložení typu předešlého tokenu
bool generator_popl_token = false;
bool int2float=false;
int posledni_pop_index=0;
char *nazev_posledni_ID;
bool pouziti_relacnich_operatoru;
int skipovani; //0-ne 1-připrav se 2-skipni
int remember_gen_place=1;
bool in_fuction_call=false;
 // simulace pravidla <program> -> <st-list>
int program(){
	if(!already_init){
		init_parser();
		generator_init();
		gen_to_main = 1;
	}
	int res; // vyhodnocovaní chyb pro parser
	
	if (set_token_and_return()!=END){
		//printf("VYCISTIL SE ZASOBNIK JADADADADADADADADADADAD\n");
		znic_zasobniky();
	}
	switch(token_type){	
		case LEFT_BRACKET:

		case TYPE_INT:
		case TYPE_FLOAT:
		case TYPE_STRING:

			res = rule_expresion_pusher();
			if(is_err(res) != NO_ERROR){
				return res;
			}
		break;
		case DEF:
			uvnitr_funkce++;
			if (uvnitr_funkce > 1){
				call_generator(ERROR_2);
				return ERROR_2;
			}
			res = rule_def();
			if(is_err(res) != NO_ERROR){
				uvnitr_funkce--;
				call_generator(res);
				return res;
			}else{
				uvnitr_funkce--;
				call_generator(res);
			}
		break; 
		case END_OF_FILE:
			gen_main_end();
			print_output();	
			return NO_ERROR;
		break;
		case END_OF_LINE:
			res = program();
			return res;
		break;
		case TYPE_IDENTIFIER:
			res=zjisti_co_je_id();	
			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}
		break;
		default: 			
			res = rule_KW();
			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}else{
				call_generator(res);
			}

	}
	program();
	return NO_ERROR;
}
void znic_zasobniky(){
	while(!SEmpty(&stack)){
		SPop(&stack);
	}
	while(!SEmpty(&operatory)){
		SPop(&operatory);
	}
	while(!TEmpty(&values)){
		TPop(&values);
	}
}
void call_generator(int resu){
	if (is_err(resu)!=NO_ERROR){
		printf("vyskytl se error: %d \n", resu );
		exit(-1);
	}
	else{
		//printf("zavolam generator pro posledni radek \n");
	}
}

int rule_KW(){
	int result=NO_ERROR;
	switch(token_type){
		case IF:
			deep_in_program++;
			last_if_counter=deep_in_program;
			last_KW=IF;
			set_token_and_return();
			if (token_type!=THEN){
				result=rule_expresion_pusher();
				if (is_err(result)!=NO_ERROR){
					return result;
				}
			}
			else {
				return ERROR_2;
			}
			//pokud se to rovná then tak podmínka je nil ?? 
			if(token_type!=THEN){
				return ERROR_2;
			}
			if (set_token_and_return()!=END_OF_LINE){
				return ERROR_2;
			}
			if (typ_promene!=NIL && typ_promene!=WITHOUT_TYPE && pouziti_relacnich_operatoru==false){
				push_true();
				gen_stack_pop("GF","result");
			}
			gen_if_start();
			return NO_ERROR;

		break;
		case ELSE:
			gen_else();
			if (deep_in_program!=last_if_counter && last_KW==ELSE){
				return ERROR_2;
			}
			if (set_token_and_return()!=END_OF_LINE){
				return ERROR_2;
			}
			last_KW=ELSE;

			return NO_ERROR;
		break;
		case WHILE:
			if (gen_to_main != 2)
			{
				remember_gen_place=gen_to_main;	
			}
			gen_to_main=2;//generovani do specialniho stringu
			gen_while_start();

			deep_in_program++;
			last_KW=WHILE;
			if (set_token_and_return()!=END_OF_LINE){//pokud se to rovná end_of_line tak podmínka je nil ??
				result=rule_expresion_pusher();
				if (is_err(result)!=NO_ERROR){
					return result;
				}
			}else{
				return ERROR_2;
			}
			gen_while_condition();
			if (token_type!=END_OF_LINE){
				return ERROR_2;
			}
			return NO_ERROR;
			
		break;
		case END:
			last_KW=END;
			printf("last_if_counter : %d\n",last_if_counter);
			printf("deep_in_program : %d\n", deep_in_program);
			//printf("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiifffffffffffffffffffffffffffffffffffj\n");
			if (deep_in_program==0){
				//tady musíme dělat něco ? :D 
				//printf("zakončil jsem deklaraci funkce\n");
				if(uvnitr_funkce==1){
					result=check_end();
					if (is_err(result)!=NO_ERROR){
						return result;
					}
				}
				else{
					return ERROR_2;
				}
			}
			else{
				
				if(last_if_counter==deep_in_program && if_counter>=1){
					last_if_counter--;
					deep_in_program--;
					result=check_end();
					if (is_err(result)!=NO_ERROR){
						return result;
					}
					gen_if_end();
					return NO_ERROR;
				}
				else {
					deep_in_program--;
					if (while_counter==1){
						gen_to_main=remember_gen_place;
					}
					gen_while_end();
				}
				
				return NO_ERROR;	
			}
		default:
			return rule_preset_fuctions();


	}
}
int rule_preset_fuctions(){
	int result=NO_ERROR;
	int temp1=brackets_counter;
	brackets_counter=0;
	switch(token_type){
		case PRINT:
			if (set_token_and_return()==LEFT_BRACKET){
				uzavorkovana_funkce=true;
				brackets_counter++;
				if(set_token_and_return() < TYPE_IDENTIFIER && token_type > TYPE_STRING ){
					return ERROR_5;//volani execute funkce
				}
			}
			else if(token_type < TYPE_IDENTIFIER && token_type > TYPE_STRING){
				return ERROR_5;
			}

			result=rule_print();
			if (is_err(result)!=NO_ERROR){
				return result;//volani execute funkce
			}
			if (brackets_counter!=0){
				printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa\n");
				return ERROR_2;//volani execute funkce
			}
			brackets_counter=temp1;
			uzavorkovana_funkce=false;
			return NIL;
		break;
		case LENGHT:
			if (set_token_and_return()==LEFT_BRACKET){
				brackets_counter++;
			}
			if (set_token_and_return()==TYPE_IDENTIFIER){
				typ_promene=TYPE_STRING;
				if(zjisti_co_je_id()!=TYPE_STRING){
					return ERROR_5; //volani execute funkce
				}	
			}
			else if(token_type==TYPE_STRING){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type==RIGHT_BRACKET ){
				brackets_counter--;
			}
			if(brackets_counter==0){
				brackets_counter=temp1;
				return TYPE_INT;
			}
		break;
		case SUBSTR:
			if (set_token_and_return()==LEFT_BRACKET){
				brackets_counter++;
			}
			if (set_token_and_return()==TYPE_IDENTIFIER){
				typ_promene=TYPE_STRING;
				if(zjisti_co_je_id()!=TYPE_STRING){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
					return ERROR_5; //volani execute funkce
				}	
			}
			else if(token_type==TYPE_STRING){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type!=COMMA && (set_token_and_return()!=TYPE_INT || token_type!=TYPE_IDENTIFIER)){
				return ERROR_5; //volani execute funkce
			}
			if (token_type==TYPE_IDENTIFIER){
				typ_promene=TYPE_INT;
				if(zjisti_co_je_id()!=TYPE_INT){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
				return ERROR_5; //volani execute funkce
				}		
			}
			else if(token_type==TYPE_INT){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type!=COMMA && (set_token_and_return()!=TYPE_INT || token_type!=TYPE_IDENTIFIER)){
				return ERROR_5; //volani execute funkce
			}
			if (token_type==TYPE_IDENTIFIER){
				typ_promene=TYPE_INT;
				if(zjisti_co_je_id()!=TYPE_INT){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
				return ERROR_5; //volani execute funkce
				}		
			}
			else if(token_type==TYPE_INT){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type==RIGHT_BRACKET ){
				brackets_counter--;
			}
			if(brackets_counter==0){
				brackets_counter=temp1;
				return TYPE_STRING;//TADY SE MUSIME DOMLUVIT NA TOM ze budu vract podle toho co mi vrati dan :D bud NIL nebo STRING
			}

		case ORD:
			if (set_token_and_return()==LEFT_BRACKET){
				brackets_counter++;
			}
			if (set_token_and_return()==TYPE_IDENTIFIER){
				typ_promene=TYPE_STRING;
				if(zjisti_co_je_id()!=TYPE_STRING){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
					return ERROR_5; //volani execute funkce
				}	
			}
			else if(token_type==TYPE_STRING){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type!=COMMA && (set_token_and_return()!=TYPE_INT || token_type!=TYPE_IDENTIFIER)){
				return ERROR_5; //volani execute funkce
			}
			if (token_type==TYPE_IDENTIFIER){
				typ_promene=TYPE_INT;
				if(zjisti_co_je_id()!=TYPE_INT){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
				return ERROR_5; //volani execute funkce
				}		
			}
			else if(token_type==TYPE_INT){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type==RIGHT_BRACKET ){
				brackets_counter--;
			}
			if(brackets_counter==0){
				brackets_counter=temp1;
				return TYPE_INT;//TADY SE MUSIME DOMLUVIT NA TOM ze budu vract podle toho co mi vrati dan :D bud NIL nebo INT
			}
		break;
		case CHR:
			if (set_token_and_return()==LEFT_BRACKET){
				brackets_counter++;
			}
			if (set_token_and_return()==TYPE_IDENTIFIER){
				typ_promene=TYPE_INT;
				if(zjisti_co_je_id()!=TYPE_INT){
					//pridat do funkce jeste jednu promenou kdyz bude vracet string, tak at se ulozi delka 
					return ERROR_5; //volani execute funkce
				}	
			}
			else if(token_type==TYPE_INT){
				set_token_and_return();
			}
			else{
				return ERROR_5; //volani execute funkce
			}
			if(token_type==RIGHT_BRACKET ){
				brackets_counter--;
			}
			if(brackets_counter==0){
				brackets_counter=temp1;
				return TYPE_STRING;//TADY SE MUSIME DOMLUVIT NA TOM ze budu vract podle toho co mi vrati dan :D bud NIL nebo STRING
			}	
		break;
		default:
			return ERROR_2; // tady je myšleno když to nebude cokoliv co tady máme uvedeno 
	}
}

int rule_print(){
	int result=NO_ERROR;
	switch(token_type){
		case COMMA:
			if((set_token_and_return() < TYPE_IDENTIFIER && token_type > TYPE_STRING ) && (token_type!=LEFT_BRACKET)){
				printf("AAAAAAAAAAaa%d\n",token_type);
				return ERROR_5;
			}

			result=rule_print();
			if (is_err(result)!= NO_ERROR){
				return result;
			}
		break;
		case TYPE_STRING:
		case TYPE_IDENTIFIER:
		case TYPE_INT:
		case TYPE_FLOAT:
		case LEFT_BRACKET:
			if (token_type==LEFT_BRACKET){
				brackets_counter++;
			}
			result = rule_expresion_pusher();
			if (is_err(result)!=NO_ERROR){
				return result;
			}
			create_frame();
			gen_call("print");
			znic_zasobniky();
			if (token_type==RIGHT_BRACKET && brackets_counter==1){
				brackets_counter--;
				set_token_and_return();
				return NO_ERROR;
			}
			else if (token_type==COMMA){
				result=rule_print();
				if (is_err(result)!=NO_ERROR){
					return result;
				}
			}
			else if ((token_type==END_OF_LINE || token_type== END_OF_FILE) && brackets_counter == 0){
				return NO_ERROR;
			}

		break;
		default:
			return ERROR_5;
	}
}

int check_end(){
	if(set_token_and_return()==END_OF_LINE || token_type==END_OF_FILE){
		return NO_ERROR;					
	}
	else{
		return ERROR_2;
	}
}
int rule_expresion_pusher(){
	typ_promene=WITHOUT_TYPE;
	tStack tmp_s;
	SInit(&tmp_s);
	int result=NO_ERROR;
	remember_token_type=token_type;
	int p_tok1;
	int2float=false;
	int top_stack = I_DOLLAR;
	pouziti_relacnich_operatoru = false;
	print_stack(&stack);
	if (SEmpty(&stack)){
		print_stack(&stack);
		SPush(&stack, top_stack);	
	}
	generator_popl_token=false;
	bool is_last_NONTERM = false;
	p_tok1 = get_prec_table_index(token_type);
/*frida*/	if (token_type!=TYPE_IDENTIFIER && (token_type>=TYPE_INT && token_type<=TYPE_STRING)){
				if (is_err(set_type_promene(token_type))!=NO_ERROR){
/**/				return set_type_promene(token_type);
/**/			}
/**/		}
/**/		else if (token_type==TYPE_IDENTIFIER){

/**/			int IDecko=check_id();
/**/			if (is_err(set_type_promene(IDecko))!=NO_ERROR){
/**/				return set_type_promene(IDecko);
/**/			}
/**/			
/**/
/*frida*/	}
	//printf("%d\n",p_tok1 );
	//printf("%d\n",top_stack );
	while(!(top_stack == I_DOLLAR && p_tok1 == I_DOLLAR)){
		//printf("CYKLUS, TOKEN: %d\n", token_type);
		////printf("%d %d\n",STop(&stack), p_tok1 );

		//Pokud je na vrcholu nonterm, nepracuje se s nontermem ale symbolem pred nim (viz prednaska 8)
		if(STop(&stack)==NONTERM){
		//	printf("Na vrcholu zásobníku je NONTERM\n");
			is_last_NONTERM = true;
			SPop(&stack);
			top_stack = STop(&stack);
			SPush(&stack, NONTERM);
		}else{			
			top_stack = STop(&stack);
			is_last_NONTERM = false;
		}

		if((top_stack == I_DOLLAR) && (p_tok1 == I_DOLLAR)){
			if (generator_popl_token){
				gen_stack_pop("GF","result");
			}
			/*create_frame();
			gen_call("print");*/
			//gen_clear(); cisteni zasobníku
			break;
		}
		print_stack(&stack);

		if(prior[top_stack][p_tok1]==N){
			
			if (brackets_counter >0 && generator_popl_token==false && (token_type== RIGHT_BRACKET || token_type==COMMA)){
				printf("AAAAAAAAAAAAAAAAAAAaa\n");
				gen_stack_push(TPop(&values));
				gen_stack_pop("GF","result");
				znic_zasobniky();
				return NO_ERROR;

			}else if(brackets_counter >0 && generator_popl_token==true && (token_type== RIGHT_BRACKET || token_type==COMMA)){
				gen_stack_pop("GF","result");
				znic_zasobniky();
				return NO_ERROR;
			}
			if (top_stack==7 && p_tok1==5 && uzavorkovana_funkce==true){
				gen_stack_pop("GF","result");
				znic_zasobniky();
				return NO_ERROR;
			}
			printf("ERROR: nedefinovaná operace v tabulce priorit!!!%d %d\n",top_stack, p_tok1);
			return ERROR_IDK; //NEVIIIIIIIIIIIIIIIIIIIIIM JAKÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝ ERROR
		}else if(prior[top_stack][p_tok1]==S){//Shiftuji
			//printf("BUDU shiftovat\n");
			if(STop(&stack) != top_stack){//Na vrcholu zasobiku je NONTERM
				SPop(&stack);
				SPush(&stack, S);
				SPush(&stack, NONTERM);
				top_stack = p_tok1; //Protoze jsme shiftovali, tak na vrcholu zasobniku NEMUZE byt I_DOLLAR
			}else{
				SPush(&stack, S);				
			}		
			if(p_tok1 == I_DATA){
				if(token_type >= TYPE_IDENTIFIER && token_type <= TYPE_STRING){
					//printf("asdasaaaaa\n");
					TPush(&values, *token);
				}
			/*	else if(token_type == TYPE_FLOAT){
					SPush(&values, token->attribute.decimal);
				}
				else if(token_type == TYPE_IDENTIFIER){
					SPush(&values, token->attribute.string->word);
				}
				else if(token_type == TYPE_STRING){
					SPush(&values, token->attribute.string->word);
				}*/
				SPush(&operatory, 0);
				print_token_stack(&values);
			}
			else if((token_type>=PLUS && token_type <= MUL) || (token_type >= COMPARE&& token_type <= NOTEQUAL)){

				if (pouziti_relacnich_operatoru){
					skipovani++;
					generator_popl_token=false;
					pouziti_relacnich_operatoru=false;
				}
				if (token_type >= COMPARE&& token_type <= NOTEQUAL){
					pouziti_relacnich_operatoru=true;
				}
				if (!pouziti_relacnich_operatoru && skipovani == 0){
					skipovani++;
				}
				SPush(&operatory, token_type);
		  	}
		
			if (brackets_counter==0 && token_type== RIGHT_BRACKET && uzavorkovana_funkce==true){
				brackets_counter++;
				printf("AAAAAAAAAAAAAAAAAAAAAAa\n");
			}
			else{
				printf("AAAAAAAAAAAAAAAAAAAAAAa%d \n", token_type);
				SPush(&stack, p_tok1);
			}
			print_stack(&stack);
			if(remember_token_type == I_DOLLAR){
				remember_token_type = END_OF_FILE;
				printf("ANO\n");
				return ERROR_IDK;
				break;
			}else if (set_token_and_return() == END_OF_FILE || token_type == THEN ||
/*frida*/		 token_type == DO || token_type== END_OF_LINE || token_type == COMMA || (brackets_counter>0 && token_type== RIGHT_BRACKET)){
				//printf("AAAAAAAAAAAAAAAAAAAAAAAAAAaa\n");
				remember_token_type = I_DOLLAR;
				//printf("I_DOLLAR na konci\n");
			}
			if (token_type==LEFT_BRACKET){
				brackets_counter++;
			}
			else if (token_type==RIGHT_BRACKET){
				brackets_counter--;
			}
			p_tok1 = get_prec_table_index(token_type);
			if (remember_token_type == LEFT_BRACKET && token_type==RIGHT_BRACKET )
			{
				token_type=NIL;

				push_false();
				gen_stack_pop("GF","result");
				znic_zasobniky();
				if ( typ_promene != WITHOUT_TYPE && typ_promene!=NIL){
					return ERROR_4;
				}
				set_token_and_return();

				return NO_ERROR;
			}
			remember_token_type=token_type;
/*frida*/	if (token_type!=TYPE_IDENTIFIER && (token_type>=TYPE_INT && token_type<=TYPE_STRING)){
/**/			if (is_err(set_type_promene(token_type))!=NO_ERROR){
/**/				return set_type_promene(token_type);
/**/			}
/**/		}
/**/		else if (token_type==TYPE_IDENTIFIER){
/**/			int IDecko=check_id();
/**/			if (is_err(set_type_promene(IDecko))!=NO_ERROR){
/**/				return set_type_promene(IDecko);
/**/			}
/**/		
/**/
/*frida*/	}

		}else if(prior[top_stack][p_tok1]==R){//Redukuji
			//printf("BUDU REDUKOVAT: %d %d\n", top_stack,p_tok1);
			if(top_of_stack_prepared_for_reduction(&stack)){
				//printf("redukuji :D stack:%d expr:%d\n",top_stack,p_tok1);
			}else{
				printf("FATAL ERROR WHEN REDUCING\n");
				return ERROR_IDK;
			}
			
		}else if(prior[top_stack][p_tok1]==E){
			//printf("Equal :D\n");
			if (brackets_counter==0 && token_type== RIGHT_BRACKET && uzavorkovana_funkce==true){
				brackets_counter++;
				printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaAAAAAAAAAAAAAAAAAAAa\n");

			}
			else{
				printf("AAAAAAAAAAAAAAAAAAAAAAa%d \n", brackets_counter);

				SPush(&stack, p_tok1);
			}
			

			if(remember_token_type == I_DOLLAR){
				remember_token_type = END_OF_FILE;
				printf("ANO\n");
				return ERROR_IDK;
				break;
			}else if (set_token_and_return() == END_OF_FILE || token_type == THEN ||
/*frida*/		 token_type == DO || token_type== END_OF_LINE || token_type == COMMA || (brackets_counter>0 && token_type== RIGHT_BRACKET)){

				remember_token_type = I_DOLLAR;
				//printf("I_DOLLAR na konci\n");
			}
			if (token_type==LEFT_BRACKET){
				brackets_counter++;
			}
			else if (token_type==RIGHT_BRACKET){
				brackets_counter--;
			}
				p_tok1 = get_prec_table_index(token_type);

			if (remember_token_type == LEFT_BRACKET && token_type==RIGHT_BRACKET )
			{
				token_type=NIL;

				push_false();
				gen_stack_pop("GF","result");
				znic_zasobniky();
				if ( typ_promene != WITHOUT_TYPE && typ_promene!=NIL){
					return ERROR_4;
				}
				set_token_and_return();

				return NO_ERROR;
			}
			remember_token_type=token_type;
/*frida*/	if (token_type!=TYPE_IDENTIFIER && (token_type>=TYPE_INT && token_type<=TYPE_STRING)){
/**/			if (is_err(set_type_promene(token_type))!=NO_ERROR){
/**/				return set_type_promene(token_type);
/**/			}
/**/		}
/**/		else if (token_type==TYPE_IDENTIFIER){
/**/			int IDecko=check_id();
/**/			if (is_err(set_type_promene(IDecko))!=NO_ERROR){
/**/				return set_type_promene(IDecko);
/**/			}
/**/	
/**/
/*frida*/	}

			if(top_of_stack_prepared_for_reduction(&stack)){
				//printf("equal :D stack:%d expr:%d\n",top_stack,p_tok1);
			}else{
				print_stack(&stack);
				printf("FATAL ERROR WHEN EQUAL\n");
				return ERROR_IDK;
			}
		}


	//print_stack(&stack);
		//printf("%d %d\n",top_stack,p_tok1 );
		//printf("------------------------------------------------------\n");
	}
	if (!generator_popl_token){
		gen_stack_push(TPop(&values));
		gen_stack_pop("GF","result");

	}
	//printf("KONEC řádku, tisknu vysledny zasobnik:\n");
	print_stack(&stack);
	porovnavani_counter=0;
	return NO_ERROR;
}


int set_type_promene(int typu){
	if (typu==TYPE_STRING && (typ_promene == WITHOUT_TYPE || typ_promene == TYPE_STRING)){
		typ_promene=TYPE_STRING; 
	}
	else if(typu==TYPE_INT && typ_promene!=TYPE_FLOAT && (typ_promene == WITHOUT_TYPE || typ_promene == TYPE_INT )){
		typ_promene=TYPE_INT;
	}
	else if(typu==TYPE_FLOAT && (typ_promene == WITHOUT_TYPE || typ_promene == TYPE_FLOAT || typ_promene == TYPE_INT)){
		typ_promene=TYPE_FLOAT;
	}
	else if(typu==TYPE_INT && typ_promene==TYPE_FLOAT){
		typ_promene=TYPE_FLOAT;
	}
	else if(typu==WITHOUT_TYPE){
		return NO_ERROR;
	}
	else if (typu==NIL && (typ_promene == NIL || typ_promene==WITHOUT_TYPE)){
		typ_promene=NIL;
	}
	else{
		return ERROR_4; // jde o kombinovaní prace se stringy a čísly
	}
	return NO_ERROR;
}

int check_id(){
	tKey k = ((char*)token->attribute.string->word);
	nazev_posledni_ID=k;
	if(htRead(h_tabulka,k) == NULL){
		return ERROR_3;
	}
	else{
		tData *data = malloc(sizeof(tData));
		data=htRead(h_tabulka,k);
		if(!data->funkce){
			int data_type = data->type;
			return data_type;
		}else{
			free(data);
			return ERROR_4;
		}
	}
}

int zjisti_co_je_id(){
	int result = NO_ERROR;
			tKey k = ((char*)token->attribute.string->word);
			nazev_posledni_ID=k;
			//tady bych chtěl nějak udělat abych si vyčetl data te proměné, když to nebude již založeno takže asi něco jako tData data1=htRead(&h_tabulka,k) ??? a nebo ještě alokace nebo jak ?? -- zeptat se kuby !!!
			if(htRead(h_tabulka,k) == NULL){//Pokud nebyla nalezena polozka, tak ji vloz
				if(set_token_and_return() == EQUALS){ 	
					//zde se bude jednat o definici nove promene, musí se nainicializovat na nil, potom se do ní musí přidat hodnota která je za "="
					pouziti_relacnich_operatoru=true;
					tData *data = malloc(sizeof(tData));
					data->type=NIL;
					htInsert(h_tabulka, k, data );
					if (gen_to_main==2){
						int i = gen_to_main;
						gen_to_main=remember_gen_place;
						remember_gen_place=i;
					}
					gen_var(k);	
					if (remember_gen_place==2){
						int i = remember_gen_place;
						remember_gen_place=gen_to_main;
						gen_to_main=i;
					}		
					result=rule_definice_promene();
					if(is_err(result)!=NO_ERROR){
						return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
					}

					if (typ_promene!=NIL){
						result_to_var(k);
					}
					last_id_func=false;
					data->funkce=false;
					data->type=typ_promene;
					htInsert(h_tabulka, k, data );
					return NO_ERROR;
				}
				else{				
					//zavolani jeste nedeklarovane funkce QQ
					int temp1 = pocitac_param_u_call;
					int temp2 = brackets_counter;
					bool temp3 = uzavorkovana_funkce;
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					pocitac_param_u_call=0;
					brackets_counter=0;
					last_id_func=true;
					uzavorkovana_funkce=false;
					volani_func++;
					if (token_type==LEFT_BRACKET || (token_type<=TYPE_IDENTIFIER && token_type>=TYPE_STRING)){
						result = rule_param_counter();
						if(is_err(result)!=NO_ERROR){
							return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
						}
					}
					data->pocet_parametru=pocitac_param_u_call;
					pocitac_param_u_call = temp1;
					if (brackets_counter==-1){
						brackets_counter = temp2 - 1;
					}
					else{
						brackets_counter = temp2;
					}
					if (uzavorkovana_funkce== false && temp3 == true){
						uzavorkovana_funkce= false;
					}else{
						uzavorkovana_funkce = temp3;
					}
					volani_func--;
					data->definovano=false;
					typ_promene=NIL;
					last_id_func=true;	
					data->funkce=true;
					htInsert(h_tabulka, k, data );
					return NO_ERROR;
				}
			}else{
				tData *data2 = malloc(sizeof(tData));
				data2=htRead(h_tabulka,k);
				tmp=token;
				/*if(set_token_and_return() == EQUALS){
					////printf("redefinice promene token_type %d\n",token_type);
					//nové přiřazování do proměné, hádám když můžem přetypovávat i znova a znova redefinovat proměnou tak se nic nestane když jenom 					
					result=rule_definice_promene();
					if(is_err(result)!=NO_ERROR){
						return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
					}
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					last_id_func=false;
					data->funkce=false;
					data->type=typ_promene;	
					htInsert(h_tabulka, k, data );
					return typ_promene;*/
				if(data2->funkce){
					int temp1 = pocitac_param_u_call;
					int temp2 = brackets_counter;
					bool temp3 = uzavorkovana_funkce;
					pocitac_param_u_call=-(data2->pocet_parametru);
					if (data2->pocet_parametru==0){
						return data2->type;
					}
					brackets_counter=0;
					last_id_func=true;
					uzavorkovana_funkce=false;
					volani_func++;
					if (token_type==LEFT_BRACKET || (token_type<=TYPE_IDENTIFIER && token_type>=TYPE_STRING)){
						result = rule_param_counter();
						if(is_err(result)!=NO_ERROR){
							return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
						}
					}	
					if(pocitac_param_u_call!=0){
						//printf("AAAAAAAAAAAAAAAAa\n");
						return ERROR_5;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
						
					}	
					pocitac_param_u_call = temp1;
					if (brackets_counter==-1){
						brackets_counter = temp2 - 1;
					}
					else{
						brackets_counter = temp2;
					}
					if (uzavorkovana_funkce == false && temp3 == true){
						uzavorkovana_funkce = false;
					}else{
						uzavorkovana_funkce = temp3;
					}
					volani_func--;
					return NO_ERROR;
				}
				else{
					//printf("QQQQQQQQQQQQQQQQQAAAAAAAAAAAAAAAAaQQQQQQQQQQQQQQQQQQQQQQQQQQQqq\n");

					//jenom pouziti promene, kontrolovat něco je asi zbytečné
					last_id_func=false;
					typ_promene=data2->type;
					SPush(&stack,I_DOLLAR);
					SPush(&stack,NONTERM);
					TPush(&values, *token);

					if (set_token_and_return()==EQUALS){
						pouziti_relacnich_operatoru=true;
						SPop(&stack);
						SPop(&stack);
						TPop(&values);
						result=rule_definice_promene();
						if(is_err(result)!=NO_ERROR){
							return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
						
						}
						result_to_var(k);
						//generovat movnoti do promene vysledek posledního result
						data2->type=typ_promene;
						htInsert(h_tabulka, k, data2 );
						return NO_ERROR;
					}
					else if((token_type>=PLUS && token_type <= MUL) || (token_type >= COMPARE&& token_type <= NOTEQUAL)){	
						//generator hod na stack x
						rule_expresion_pusher();

						return NO_ERROR;
					}
					else if (token_type==END_OF_LINE || token_type==END_OF_FILE){

						return NO_ERROR;
					}
					else {
						return ERROR_2;
					}
				}
			}
return -1;
}

int rule_param_counter(){
	int result=NO_ERROR;
	switch(token_type){
		case LEFT_BRACKET:
			brackets_counter++;
			uzavorkovana_funkce=true;
			set_token_and_return();
			result=rule_param_counter();
			return result;
		break;
		case RIGHT_BRACKET:
			brackets_counter--;
			set_token_and_return();
			if (brackets_counter==0){
				uzavorkovana_funkce=false;
				return NO_ERROR;
			}
			else if(brackets_counter==-1 && volani_func > 1 ){
				uzavorkovana_funkce=false;
				return NO_ERROR;
			}
			else{
				return ERROR_2;
			}
		break;
		case TYPE_IDENTIFIER:
		case TYPE_INT:
		case TYPE_FLOAT://tady budem muset asi upravit na volani expr při +- apod. ........ i wanna kill my self
			result = rule_expresion_pusher();
			if (is_err(result)!=NO_ERROR){
				return result;
			}
			if (token_type == COMMA ){
				pocitac_param_u_call++;
				result=rule_param_counter();
				return result;
			}
			else if (token_type == END_OF_LINE || token_type == RIGHT_BRACKET){
				result=rule_param_counter();
				return result;
			}else{
				return ERROR_2;
			}
		break;
		case COMMA:
			if (pocitac_param_u_call==0){
				return NO_ERROR;
			}
			else if ((set_token_and_return() >= TYPE_IDENTIFIER) && (token_type <= TYPE_STRING) ){
				result=rule_param_counter();
				return result;
			}
			else{
				return ERROR_2;
			}
		break;
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
			if ((set_token_and_return() >= TYPE_IDENTIFIER) && (token_type <= TYPE_STRING) ){
				result=rule_param_counter();
				return result;
			}
			else{
				return ERROR_2;
			}
		case END_OF_LINE:
			//set_token_and_return();
			if (brackets_counter<=0 && volani_func==1 && uzavorkovana_funkce==false){
				return NO_ERROR;
			}
			else{
				return ERROR_2;
			}
		break;

	}
	return -1;

}

int rule_definice_promene(){
	int result=NO_ERROR;
	typ_promene=WITHOUT_TYPE;//není to zatím zadny typ
	set_token_and_return(); 
	switch(token_type){
		case TYPE_IDENTIFIER:
		{
			tKey k = ((char*)token->attribute.string->word);
			tData *data2 = malloc(sizeof(tData));
			data2=htRead(h_tabulka,k);
			if (data2==NULL){
				return ERROR_2;
			}
			else if(data2->type==NIL){
				typ_promene=NIL;
				if(set_token_and_return()!=END_OF_LINE && token_type!=END_OF_FILE){
					return ERROR_2; // možná 4 ?? IDK
				}
				else{
					return NO_ERROR;
				}
			}
			else{
				result=rule_expresion_pusher();
				return result;
			}

		break;
		}
		case TYPE_INT:
		case TYPE_FLOAT:
		case TYPE_STRING:
		print_stack(&stack);
		print_stack(&operatory);
			result=rule_expresion_pusher();
			return result;
		break;
		default:
			return ERROR_2;
			
	}
return -1;
}

int rule_def(){
	int result = NO_ERROR;
	switch(token_type){
		case DEF:
			if(set_token_and_return() != TYPE_IDENTIFIER && deep_in_program!=0){
				result = ERROR_2;
				return result;
			}
			else{

				tKey k = ((char*)token->attribute.string->word);
				last_function=k;
				tData *data2 = malloc(sizeof(tData));
				data2=htRead(h_tabulka,k);
				param_counter=0;
				if(data2 == NULL || data2->definovano==false){//Pokud nebyla nalezena polozka, tak ji vloz	
					result=rule_def();
					if(is_err(result)!=NO_ERROR){
						return result;
					}
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					data->pocet_parametru=param_counter;
					data->definovano=true;
					data->type=typ_promene;	
					last_id_func=true;
					data->funkce=true;
					if (data2!=NULL){
						if (param_counter!=data2->pocet_parametru && data->funkce!=data2->funkce){
							if(data2->type!=TYPE_INT && data2->type!=TYPE_FLOAT && data2->type!=typ_promene){
								return ERROR_2;
							}
							if(data2->type!=TYPE_STRING && ((data2->type!=typ_promene) || (data2->type+1!=typ_promene))){
								return ERROR_2;
							}
						}
					}

					htInsert(h_tabulka, k, data );
					result = NO_ERROR;

					return result;
				}else{	
					//printf("tato funkce už existuje nemůžete ji redefinovat\n");				
					return ERROR_2;
				}
			}
		break;
		case TYPE_IDENTIFIER:
			if (brackets_counter==0){
				if(set_token_and_return() != LEFT_BRACKET){
					result = ERROR_2;
					return result;
				}
				else{
					brackets_counter++;
					result=rule_def();
					return result;
				}
			}
			if (brackets_counter==1){
				if(set_token_and_return() != COMMA && token_type != RIGHT_BRACKET){
					result = ERROR_2;
					return result;
				}
				else if(token_type == COMMA){

					param_counter++;
				}
				else {
					brackets_counter--;
				}
				result=rule_def();
				return result;
			}
			else{
				result = ERROR_2;
				return result;
			}
		break;
		case LEFT_BRACKET:
			if (brackets_counter==1){
				if(set_token_and_return() != TYPE_IDENTIFIER){
					result = ERROR_2;
					return result;
				}
				else{
					result=rule_def();
					return result;
				}
			}
			else{
				result = ERROR_2;
				return result;
			}
		break;
		case RIGHT_BRACKET:
			if (brackets_counter==0){
				if(set_token_and_return() != END_OF_LINE){
					result = ERROR_2;
					return result;
				}
				//printf("tady se zavola generator na generovani hlavičky funkce def nazev_funkce (param1, param2,....,paramN) \n");
				result=program();
				//printf("Uspěšné ukončení funkce, generuj end funkce\n");
				return result;
				

			}
			else{
				result = ERROR_2;
				return result;
			}
		break;
		case COMMA:
			if (brackets_counter==1){
				if(set_token_and_return() != TYPE_IDENTIFIER){
					result = ERROR_2;
					return result;
				}
				result=rule_def();
				return result;
			}
			else{
				result = ERROR_2;
				return result;
			}
		break;
	}
return -1;
}



int rule_expr(){
	int result;
	int typ = TYPE_INT;
	switch(token_type){
		case PLUS:
		case MINUS:
			if(SEmpty(&stack)){
				result = NO_ERROR;
				return result;
			}
			pop_token();
			if(!is_num() && token_type != LEFT_BRACKET){
				result = ERROR_4;
				return result;
			}
			result=rule_expr();
			return result;
		break;
		case COMPARE:
		case LOE:
		case LESSTHAN:
		case MOE:
		case MORETHAN:
		case NOTEQUAL:
			porovnavani_counter++;
		case DIV:
		case MUL:
			if (porovnavani_counter>1){
				return ERROR_4;
			}
			if(SEmpty(&stack)){
				result = ERROR_4;
				return result;
			}
			pop_token();
			if(!is_num()){
				result = ERROR_4;
				return result;
			}
			result=rule_expr();
			return result;
		break;
		case TYPE_FLOAT:
			typ_promene=TYPE_FLOAT;
		case TYPE_INT:
		case TYPE_IDENTIFIER:
			if (token_type==TYPE_IDENTIFIER){	
				typ=zjisti_co_je_id();
			}
			if (typ_promene!=TYPE_FLOAT && typ==TYPE_INT){
				typ_promene=TYPE_INT;		
			}
			else{
				typ_promene=TYPE_FLOAT;
			}
			if(SEmpty(&stack)){
				result = NO_ERROR;
				return result;
			}
			pop_token();
			if(!is_operator()){
				result = ERROR_4;
				return result;
			}
			result=rule_expr();
			return result;
		break;
	}
return -1;
}

int is_err(int ret){
	switch(ret){
		case ERROR_1:
		case ERROR_2:
		case ERROR_3:
		case ERROR_4:
		case ERROR_5:
		case ERROR_6:
		case ERROR_9:
		case ERROR_99:
		case ERROR_IDK:
			return ret;
		break;
		default:
			return NO_ERROR;
		break;
	}
return -1;
}

bool is_operator(){
	return ((token_type >= PLUS && token_type <= MUL)||(token_type >= COMPARE && token_type <= NOTEQUAL));
}

bool is_num(){
	if (token_type==TYPE_IDENTIFIER){
		int typ =zjisti_co_je_id();
		return (typ==TYPE_INT || typ==TYPE_FLOAT);
	}

	return (token_type == TYPE_INT || token_type == TYPE_FLOAT );

}

void init_parser(){
	tmp = malloc(sizeof(token_t));
	token = malloc(sizeof(token_t));
	brackets_counter = 0;
	SInit(&stack);
	SInit(&values);
	SInit(&operatory);
	h_tabulka = (tHTable*) malloc ( sizeof(tHTable) );
	htInit(h_tabulka);
	already_init=true;
}

int set_token_and_return(){
	if (skipni_get_token){
		skipni_get_token=false;
		return token_type;
	}else{
		get_token(token);
		token_type = token->type;
		////printf("%d\n",token_type );
		return token_type;
	}

}

int pop_token(){
	token_type = SPop(&stack);
	////printf("POP_TOKEN: %d\n",token_type);
	return token_type;
}
bool top_of_stack_prepared_for_reduction(tStack *stack){

	//printf("JSEM VE FUNKCI top_of_stack_prepared_for_reduction\n");
	int i;
	for(i = 0; ;i++){
		if(stack->a[(stack->top)-i] == S){
			//printf("%d:%d\n",i, stack->a[(stack->top)-i]);

			print_stack(stack);
			break;
		}
	}

	//printf("I JE::::::::::::::::%d\n", i);
	//switch(get_rule_from_stack(i)){
	switch(i){
		// E->i
		case 1:

			SPop(stack);
			SPop(stack);
			SPush(stack, NONTERM);
			//printf("ZAS:\n");
			print_stack(stack);
			return true;

		break;
		case 3:

			if(stack->a[(stack->top)] == NONTERM && stack->a[(stack->top)-2] == NONTERM){ // E -> E + E a E -> E - E			
				if(stack->a[(stack->top)-1] == I_PLUS_MINUS){
					check_data_type();
					if (posledni_pop_index>((&values)->top)){
						switch_stack();
					}
					posledni_pop_index=((&values)->top);
					if((&operatory)->a[((&operatory)->top)-1] == PLUS){
						if (typ_promene==TYPE_STRING){
							SPop (&operatory);
							SPop (&operatory);	
							//printf("GENERUJI KONKATENACI\n");
							gen_stack_concatanate();
						}else{
							SPop (&operatory);
							SPop (&operatory);	
							//printf("GENERUJI SOUCET\n");
							gen_stack_add();
						}

						//gen_stack_pop("GF","result");
					}
					else if((&operatory)->a[((&operatory)->top)-1] == MINUS){
						if (typ_promene==TYPE_STRING){
							return ERROR_4;
						}
						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI ODECET\n");					
						gen_stack_sub();
						//gen_stack_pop("GF","result");
					}

					do_E_rule(stack);
					//printf("Zasobnik po provedení pravidla E->E+E\n");
					return true;
				}else if(stack->a[(stack->top)-1] == I_MUL_DIV){ // E -> E * E a E -> E / E					
					check_data_type();
					if (typ_promene==TYPE_STRING){
						return ERROR_4;
					}
					if (posledni_pop_index>((&values)->top)){
						switch_stack();
					}
					posledni_pop_index=((&values)->top);
					if((&operatory)->a[((&operatory)->top)-1] == MUL){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_stack_mul();
						//gen_stack_pop("GF","result");
					}
					else if((&operatory)->a[((&operatory)->top)-1] == DIV){	
						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI DELENI\n");						
						gen_stack_idiv();
						//gen_stack_pop("GF","result");
					}

					do_E_rule(stack);
					//printf("Zasobnik po provedení pravidla E->E+E\n");
					return true;
				}else if(stack->a[(stack->top)-1] == I_REL_OP){ // E -> E relacni-operator E 
					if (skipovani==2){
						skipovani=0;
					}else{
						check_data_type();	
					}
					if (posledni_pop_index>((&values)->top)){
						switch_stack();
					}
					posledni_pop_index=((&values)->top);
					if((&operatory)->a[((&operatory)->top)-1] == COMPARE){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_stack_eq();
						//gen_stack_pop("GF","result");
					}
					if((&operatory)->a[((&operatory)->top)-1] == LOE){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_less_or_equal();
						//gen_stack_pop("GF","result");
					}
					if((&operatory)->a[((&operatory)->top)-1] == LESSTHAN){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_stack_less_than();
						//gen_stack_pop("GF","result");
					}
					if((&operatory)->a[((&operatory)->top)-1] == MOE){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_more_or_equal();
						//gen_stack_pop("GF","result");
					}
					if((&operatory)->a[((&operatory)->top)-1] == MORETHAN){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_stack_more_than();
						//gen_stack_pop("GF","result");
					}
					if((&operatory)->a[((&operatory)->top)-1] == NOTEQUAL){	

						SPop (&operatory);
						SPop (&operatory);	
						//printf("GENERUJI NASOBENI\n");				
						gen_not_equal();
						//gen_stack_pop("GF","result");
					}
					do_E_rule(stack);
					//printf("Zasobnik po provedení pravidla E->E+E\n");
					return true;
				}
			//E->(E)
			}else if(stack->a[(stack->top)] == I_RIGHT_BRACKET && stack->a[(stack->top)-2] == I_LEFT_BRACKET){
				do_E_rule(stack);
				print_stack(stack);
			}else{
				//printf("Chyba při kontrole vrcholu zásobníku\n");
				return false;
			}
			
		break;
		default:
			//printf("Něco se podělalo\n");
			return false;

		break;
	}

	//printf("KONČÍM FUNKCI top_of_stack_prepared_for_reduction\n");
	return true;
}
void check_data_type(){

	if (!int2float){
		if ((TTop(&values)).type == TYPE_FLOAT && generator_popl_token==true){
			int2float=true;
			gen_int2float();
			gen_stack_push(TPop(&values));
		}
		else if((TTop(&values)).type == TYPE_FLOAT && generator_popl_token==false){
			int2float=true;
			gen_stack_push(TPop(&values));
			generator_popl_token = true;
			posledni_pop_index=((&values)->top);
			if ((TTop(&values)).type == TYPE_INT){
				gen_stack_push(TPop(&values));
				gen_int2float();
			}
			else{
				gen_stack_push(TPop(&values));
			}
		}
		else{
			gen_stack_push(TPop(&values));
			if (!generator_popl_token){
				posledni_pop_index=((&values)->top);
				if((TTop(&values)).type == TYPE_FLOAT && int2float==false ){
					gen_int2float();
					gen_stack_push(TPop(&values));
					int2float=true;
					generator_popl_token=true;
				}
				else{
					gen_stack_push(TPop(&values));
					generator_popl_token=true;
				}
			}
		}
	}
	else if (int2float==true && (TTop(&values)).type == TYPE_INT){
		gen_stack_push(TPop(&values));
		gen_int2float();
	}
	else{
		if((TTop(&values)).type == TYPE_FLOAT){
			int2float=true;
		}
		gen_stack_push(TPop(&values));

		if (!generator_popl_token){
			posledni_pop_index=((&values)->top);
			if((TTop(&values)).type == TYPE_FLOAT && int2float==false ){
				gen_int2float();
				gen_stack_push(TPop(&values));
				int2float=true;
				generator_popl_token=true;
			}
			else{
				gen_stack_push(TPop(&values));
				generator_popl_token=true;
			}
		}
	}	
}

void do_E_rule(tStack *stack){
	SPop(stack);
	SPop(stack);	
	SPop(stack);	
	SPop(stack);	
	SPush(stack, NONTERM);			
	print_stack(stack);
}


void print_stack(tStack *stack){

	printf("TISKNU OBSAH ZÁSOBNÍKU:\n");
	for(int i = 0;i< (stack)->top+1;i++){
		printf("%d ", (stack)->a[i]);
	}
	printf("\nKONEC ZÁSOBNÍKU\n");
}
void print_token_stack(Token_Stack *st){
	//printf("TISKNU OBSAH ZÁSOBNÍKU TOKENůůů:\n");
	for(int i = 0;i< (st)->top+1;i++){
		//printf("%d ", (st)->a[i].type);
	}
	//printf("\nKONEC ZÁSOBNÍKU\n");
}