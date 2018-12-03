#include "parser.h"
 
int token_type; //aktualni token
char *token_attribute; //atribut aktualniho tokenu
token_t *token = NULL;
token_t *tmp = NULL;//zjisteni dalsiho tokenu -> z toho budem vedet jestli se vola o volani nedefinovane funkce, nebo jestli se jedna o definici proměne
tStack stack; //Zasobnik pro vyrazy
tHTable* h_tabulka; // Hashovaci tabulka
int brackets_counter; //Leve zavorky ++, prave --
int param_counter; //pocitadlo parametru pri definici funkce
int porovnavani_counter;//pocitadlo >,<,<=,>=,==,!=
bool already_init=false;//podmínka pro inicializaci parseru
int uvnitr_funkce=0;//pro orientaci jestli jsme v definic funkce nebo ne 
int typ_promene;//typ promene
bool uzavorkovana_funkce=false; // pro volani funkce flag na poznani jestli je funkce uzavorkovana nebo ne 
int pocitac_param_u_call=0;//pocitadlo argumentu pro volani funkce
bool last_id_func;//jestli poslední identifikator byla funkce nebo promena
int volani_func = 0;//pocet vnoreni volani funkci
int last_KW=WITHOUT_TYPE;//posledni pouzite klicove slovo ( je to pouze pro kontrolu aby nebyli pod sebou 2x else)
int deep_in_program = 0;//pocitadlo pro hloubku programu ( kvuli while,if apod)
char *last_function;//nazev posledni pouzite funkce
int last_if_counter=0;//pocitadlo pro ify

// simulace pravidla <program> -> <st-list>
int program(){
	if(!already_init){
		init_parser();
	}
	int res; // vyhodnocovaní chyb pro parser
	set_token_and_return();
	switch(token_type){	
		case LEFT_BRACKET:
			brackets_counter++;
		case TYPE_INT:
		case TYPE_FLOAT:
		case TYPE_STRING:
		case TYPE_IDENTIFIER:	
			res = rule_expresion_pusher();
			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}else{

				call_generator(res);
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
			return NO_ERROR;
		break;
		case END_OF_LINE:
			res = program();
			return res;
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
void call_generator(int resu){
	if (is_err(resu)!=NO_ERROR){
		printf("vyskytl se error: %d \n", resu );
		free_all();
		exit(-1);
	}
	else{
		printf("zavolam generator pro posledni radek \n");
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
			//pokud se to rovná then tak podmínka je nil ?? 
			if(token_type!=THEN){
				return ERROR_2;
			}
			if (set_token_and_return()!=END_OF_LINE){
				return ERROR_2;
			}
			program();//musí se zakázat definice funkce 
		break;
		case ELSE:
			if (deep_in_program!=last_if_counter && last_KW==ELSE){
				return ERROR_2;
			}
			if (set_token_and_return()!=END_OF_LINE){
				return ERROR_2;
			}
			last_KW=ELSE;
		break;
		case WHILE:
			deep_in_program++;
			last_KW=WHILE;
			set_token_and_return();
			if (token_type!=END_OF_LINE){//pokud se to rovná end_of_line tak podmínka je nil ??
				result=rule_expresion_pusher();
				if (is_err(result)!=NO_ERROR){
					return result;
				}
			}
			if (token_type!=END_OF_LINE){
				return ERROR_2;
			}
			program();//musí se zakázat definice funkce
			
		break;
		case END:
			if (deep_in_program==0){
				//tady musíme dělat něco ? :D 
				printf("zakončil jsem deklaraci funkce\n");
				if(uvnitr_funkce==1){
					return check_end();
				}
				else{
					return ERROR_2;
				}
			}
			else{
				deep_in_program--;
				last_KW=END;
				if (last_KW==WHILE){
					printf("zakončíme while \n");
					return check_end();

				}
				else if(last_KW==ELSE || last_KW==IF){
					printf("zakončíme if \n");
					last_if_counter--;
					return check_end();
				}
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
				brackets_counter++;
			}
			if(set_token_and_return() < TYPE_IDENTIFIER && token_type > TYPE_STRING ){
				return ERROR_5;//volani execute funkce
			}
			result=rule_print();
			if (is_err(result)!=NO_ERROR){
				return result;//volani execute funkce
			}
			if (brackets_counter!=0){
				return ERROR_2;//volani execute funkce
			}
			brackets_counter=temp1;
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
			if(set_token_and_return() < TYPE_IDENTIFIER && token_type > TYPE_STRING ){
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
			//nacti posli do generatoru tady zase bude muset byt nějakej expresion pusher QQ
			if (set_token_and_return()==RIGHT_BRACKET && brackets_counter==1){
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
	if(set_token_and_return()==END_OF_LINE){
		return NO_ERROR;					
	}
	else{
		return ERROR_2;
	}
}
int rule_expresion_pusher(){
	tStack tmp_s;
	SInit(&tmp_s);
	int result=NO_ERROR;
	int p_tok1;

	int top_stack = I_DOLLAR;
	SPush(&stack, top_stack);

	bool is_last_NONTERM = false;
	p_tok1 = get_prec_table_index(token_type);
	printf("%d\n",p_tok1 );
	printf("%d\n",top_stack );
	while(!(top_stack == I_DOLLAR && p_tok1 == I_DOLLAR)){
		printf("CYKLUS, TOKEN: %d\n", token_type);
		//printf("%d %d\n",STop(&stack), p_tok1 );

		//Pokud je na vrcholu nonterm, nepracuje se s nontermem ale symbolem pred nim (viz prednaska 8)
		if(STop(&stack)==NONTERM){
			printf("Na vrcholu zásobníku je NONTERM\n");
			is_last_NONTERM = true;
			SPop(&stack);
			top_stack = STop(&stack);
			//printf("QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ%d\n", top_stack);
			SPush(&stack, NONTERM);
		}else{			
			top_stack = STop(&stack);
			is_last_NONTERM = false;
		}

		if((top_stack == I_DOLLAR) && (p_tok1 == I_DOLLAR)){
			printf("KONEC VÝRAZU!\n");
			break;
		}

		if(prior[top_stack][p_tok1]==N){
			printf("ERROR: nedefinovaná operace v tabulce priorit!!!%d %d\n",top_stack, p_tok1);
			return ERROR_IDK; //NEVIIIIIIIIIIIIIIIIIIIIIM JAKÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝÝ ERROR
		}else if(prior[top_stack][p_tok1]==S){//Shiftuji
			printf("BUDU shiftovat\n");
			if(STop(&stack) != top_stack){//Na vrcholu zasobiku je NONTERM
				SPop(&stack);
				SPush(&stack, S);
				SPush(&stack, NONTERM);
				top_stack = p_tok1; //Protoze jsme shiftovali, tak na vrcholu zasobniku NEMUZE byt I_DOLLAR
			}else{
				SPush(&stack, S);				
			}			
			SPush(&stack, p_tok1);
			print_stack(&stack);

			if(token_type == I_DOLLAR){
				token_type = END_OF_FILE;
				printf("ANO\n");
				return ERROR_IDK;
				break;
			}else if (set_token_and_return() == END_OF_FILE){
				token_type = I_DOLLAR;
				printf("I_DOLLAR na konci\n");
			}
			p_tok1 = get_prec_table_index(token_type);
			
		}else if(prior[top_stack][p_tok1]==R){//Redukuji
			if(top_of_stack_prepared_for_reduction(&stack)){
				printf("redukuji :D stack:%d expr:%d\n",top_stack,p_tok1);
			}else{
				printf("FATAL ERROR WHEN REDUCING\n");
				return ERROR_IDK;
			}
			
		}else if(prior[top_stack][p_tok1]==E){
			printf("Equal :D\n");
		}


	//print_stack(&stack);
		printf("%d %d\n",top_stack,p_tok1 );
		printf("------------------------------------------------------\n");
	}
	printf("KONEC řádku\n");
	porovnavani_counter=0;
	result = rule_expr();
	return result;

}

int zjisti_co_je_id(){
	int result = NO_ERROR;
			tKey k = ((char*)token->attribute.string->word);
			//tady bych chtěl nějak udělat abych si vyčetl data te proměné, když to nebude již založeno takže asi něco jako tData data1=htRead(&h_tabulka,k) ??? a nebo ještě alokace nebo jak ?? -- zeptat se kuby !!!


			if(htRead(h_tabulka,k) == NULL){//Pokud nebyla nalezena polozka, tak ji vloz
				tmp=token;
				if(set_token_and_return() == EQUALS){ 	
					//zde se bude jednat o definici nove promene, musí se nainicializovat na nil, potom se do ní musí přidat hodnota která je za "="
					tData *data = malloc(sizeof(tData));
					data->type=NIL;
					htInsert(h_tabulka, k, data );			
					result=rule_definice_promene();
					if(is_err(result)!=NO_ERROR){
						return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
					}
					last_id_func=false;
					data->funkce=false;
					data->type=typ_promene;
					htInsert(h_tabulka, k, data );
					
					return typ_promene;
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
					result = rule_param_counter();

					if(is_err(result)!=NO_ERROR){
						return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
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
					data->type=typ_promene;
					last_id_func=true;	
					data->funkce=true;
					htInsert(h_tabulka, k, data );
				}
			}else{
				tData *data2 = malloc(sizeof(tData));
				data2=htRead(h_tabulka,k);
				tmp=token;
				if(set_token_and_return() == EQUALS){
					//printf("redefinice promene token_type %d\n",token_type);
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
					return typ_promene;
				}else if(data2->funkce){
					data2->pocet_parametru;
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
					result = rule_param_counter();

					if(is_err(result)!=NO_ERROR){
						return result;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
					}
					if(pocitac_param_u_call!=0){
						return ERROR_5;//musí se volat error je blbost abychom vraceli error, když chceme vlastně jenom typ
						
					}	
					pocitac_param_u_call = temp1;
					if (brackets_counter==-1){
						brackets_counter = temp2 - 1;
					}
					else{
						brackets_counter = temp2;
					}
					if (uzavorkovana_funkce== false && temp3 == true){
						uzavorkovana_funkce== false;
					}else{
						uzavorkovana_funkce = temp3;
					}
					volani_func--;
					return data2->type;
				}
				else{
					//jenom pouziti promene, kontrolovat něco je asi zbytečné
					last_id_func=false;
					return data2->type;
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
	int typa;
	typ_promene=WITHOUT_TYPE;//není to zatím zadny typ
	set_token_and_return(); 
	switch(token_type){
		case TYPE_STRING:
			typ_promene=TYPE_STRING;
			printf("musíme přidat do proměné ten string co je za tím \n");
			if(set_token_and_return()==END_OF_LINE || token_type==END_OF_FILE){
				return NO_ERROR;
			}
			else if (token_type == PLUS){
				if (set_token_and_return()==TYPE_STRING){
					result=rule_definice_promene();
					return result;
				}
				else{
					return ERROR_2;
				}
			}
			else{
				return ERROR_2;
			}
			// co tady může všechno nasledovat vlastně ?? sčítaní stringu a nebo EOL ? 
		break;
		case TYPE_IDENTIFIER:
			typa = zjisti_co_je_id();
			if(typa==TYPE_STRING){
				typ_promene=TYPE_STRING;
				printf("musíme přidat do proměné ten string co je za tím \n");
				if(set_token_and_return()==END_OF_LINE || token_type==END_OF_FILE){
					return NO_ERROR;
				}
				else if (token_type == PLUS){
					if (set_token_and_return()==TYPE_STRING){
						result=rule_definice_promene();
						return result;
					}
					else{
						return ERROR_2;
					}
				}
				else{
					return ERROR_2;
				}
				// co tady může všechno nasledovat vlastně ?? sčítaní stringu a nebo EOL ? 
			}
			else if(typa==NIL){
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
		case TYPE_INT:
		case TYPE_FLOAT:
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
					printf("tato funkce už existuje nemůžete ji redefinovat\n");				
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
				printf("tady se zavola generator na generovani hlavičky funkce def nazev_funkce (param1, param2,....,paramN) \n");
				result=program();
				printf("Uspěšné ukončení funkce, generuj end funkce\n");
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
	h_tabulka = (tHTable*) malloc ( sizeof(tHTable) );
	htInit(h_tabulka);
	already_init=true;
}

int set_token_and_return(){
	get_token(token);
	token_type = token->type;
	//printf("%d\n",token_type );
	return token_type;
}

int pop_token(){
	token_type = SPop(&stack);
	//printf("POP_TOKEN: %d\n",token_type);
	return token_type;
}
bool top_of_stack_prepared_for_reduction(tStack *stack){

	printf("JSEM VE FUNKCI top_of_stack_prepared_for_reduction\n");
	int i;
	for(i = 0; ;i++){
		if(stack->a[(stack->top)-i] == S){
			printf("%d:%d\n",i, stack->a[(stack->top)-i]);

			print_stack(stack);
			break;
		}
	}

	printf("I JE::::::::::::::::%d\n", i);
	//switch(get_rule_from_stack(i)){
	switch(i){
		// E->i
		case 1:
			SPop(stack);
			SPop(stack);
			SPush(stack, NONTERM);
			printf("ZAS:\n");
			print_stack(stack);
			return true;

		break;
		case 3:
			//E->E+E
			if(stack->a[(stack->top)] == NONTERM && stack->a[(stack->top)-2] == NONTERM){
				if(stack->a[(stack->top)-1] == I_PLUS_MINUS){printf("AAAAA\n");
					do_E_rule(stack);
					printf("Zasobnik po provedení pravidla E->E+E\n");
					return true;
				}
				if(stack->a[(stack->top)-1] == I_MUL_DIV){
					do_E_rule(stack);
					printf("Zasobnik po provedení pravidla E->E+E\n");
					return true;
				}
			//E->(E)
			}else if(stack->a[(stack->top)] == I_RIGHT_BRACKET && stack->a[(stack->top)-2] == I_LEFT_BRACKET){

			}else{
				printf("Chyba při kontrole vrcholu zásobníku\n");
				return false;
			}
			

		break;
		default:
			printf("Něco se podělalo\n");
			return false;

		break;
	}

	printf("KONČÍM FUNKCI top_of_stack_prepared_for_reduction\n");
	return true;
}

void do_E_rule(tStack *stack){
	SPop(stack);
	SPop(stack);	
	SPop(stack);	
	SPop(stack);	
	SPush(stack, NONTERM);			
	print_stack(stack);
}

int get_rule_from_stack(int symbol_count){

	printf("JSEM VE FUNKCI get_rule_from_stack\n");
	switch(symbol_count){
		case 1:
			if((&stack)->a[((&stack)->top)-1] == S)
				if((&stack)->a[((&stack)->top)-0] == I_DATA)
					printf("KONČÍM FUNKCI get_rule_from_stack\n");
					return 1;

		break;


		case 3:



		break;
	}
}

int rules1[]={{NONTERM}};

void print_stack(tStack *stack){

	printf("TISKNU OBSAH ZÁSOBNÍKU:\n");
	for(
		int i = 0;i< (stack)->top+1;i++){
		printf("%d ", (stack)->a[i]);
	}
	printf("\nKONEC ZÁSOBNÍKU:\n");
}