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


//int result; // Pouzivame pro ulozeni navratove hodnoty z funkce (Error nebo ne error :D )
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
		case END:
			printf("zakončil jsem funkci\n");
			if(uvnitr_funkce==1){
				set_token_and_return();

				return NO_ERROR;
			}
			else{
				return ERROR_2;
			}
		break;
		case TYPE_IDENTIFIER:
			res = zjisti_co_je_id();
			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}else{
				call_generator(res);
			}
		break;

	}
	program();
}

void call_generator(int resu){
	if (is_err(resu)!=NO_ERROR){
		printf("vyskytl se error: %d \n", resu );
		exit(-1);
	}
	else{
		printf("zavolam generator pro posledni radek \n");
	}
}
int rule_expresion_pusher(){
	int result=NO_ERROR;
	while(token_type == TYPE_INT  || token_type == TYPE_FLOAT || 
					(token_type >=PLUS && token_type <= NOTEQUAL)){ //musí se dodělat že tam můžou byt proměné i funkce jako čísla 
				/*if ( funkce,která vrací TYPE_FLOAT/TYPE_FLOAT || proměné TYPE_FLOAT nebo TYPE_INT ){
					nastavíme token_type na to co vrací;
				}*/
				SPush(&stack, token_type);
				token_type = set_token_and_return();	
			}

			if(!is_num(pop_token()) && token_type != RIGHT_BRACKET){ //pokud vyraz nekončí zavorkou ani číslem				
				call_generator(ERROR_2);
				return ERROR_2;
			}
			porovnavani_counter=0;
			result = rule_expr();
			return result;

}
int zjisti_co_je_id(){
	int result = NO_ERROR;
			tKey k = ((char*)token->attribute.string.word);
			//tady bych chtěl nějak udělat abych si vyčetl data te proměné, když to nebude již založeno takže asi něco jako tData data1=htRead(&h_tabulka,k) ??? a nebo ještě alokace nebo jak ?? -- zeptat se kuby !!!
			
			
			if(htRead(h_tabulka,k) == NULL){//Pokud nebyla nalezena polozka, tak ji vloz
				tmp=token;
				if(set_token_and_return() == LEFT_BRACKET){
					printf("volani jeste nedefinovane funkce\n");
					//vlozime ji do hashtabulky a reknem že data.definovano=false; bude to stačit ?
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					data->type=NULL;	
					data->definovano=false;	

					htInsert(h_tabulka, k, data );
					printf("dosel jsem sem :D juppíííí\n");
					return NULL; 
				}
				else if(token_type == EQUALS){
					//zde se bude jednat o definici nove promene, musí se nainicializovat na nil, potom se do ní musí přidat hodnota která je za "="
					//result=rule_definice_promene();
					if(is_err(result)!=NO_ERROR){

						return result;
					}
					tData *data = malloc(sizeof(tData));
					data->type=TYPE_FLOAT;
					printf("priradil jsem typ do promene %d \n",data->type);
					htInsert(h_tabulka, k, data );

					return tmp->type;
				}
			}else{
				tmp=token;
				if(set_token_and_return() == LEFT_BRACKET){
					printf("volani již definovane funkce\n");
					//chceme zjistit jenom její id 
				}
				else if(token_type == EQUALS){
					//nové přiřazování do proměné, hádám když můžem přetypovávat i znova a znova redefinovat proměnou tak se nic nestane když jenom 
					//result=rule_definice_promene();
					
					result=NO_ERROR;
					if(is_err(result)!=NO_ERROR){
						return result;
					}
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					data->type=tmp->type;	
					htInsert(h_tabulka, k, data );
					return tmp->type;
				}


			}
}

int rule_definice_promene(){
	typ_promene=WITHOUT_TYPE;//není zo zatím zadny typ 
	switch(token_type){
		case TYPE_STRING:
			printf("musíme přidat do proměné ten string co je za tím \n");
		break;
		case TYPE_IDENTIFIER:
			if(zjisti_co_je_id()==TYPE_STRING){
				printf("musíme přidat do proměné ten string co je za tím \n");
			}
			else
		break;
			
			rule_expresion_pusher();
	}
	
	return NO_ERROR;
}

int rule_def(){
	int result = NO_ERROR;
	switch(token_type){
		case DEF:
			//printf("JSEM V DEF\n");
			if(set_token_and_return() != TYPE_IDENTIFIER){
				result = ERROR_2;
				return result;
			}
			else{
				tKey k = ((char*)token->attribute.string.word);
				if(htRead(h_tabulka,k) == NULL){//Pokud nebyla nalezena polozka, tak ji vloz	
					result=rule_def();
					if(is_err(result)!=NO_ERROR){
						return result;
					}
					tData *data = malloc(sizeof(tData)); //data pro hashovací tabulku
					//data.typ_funkce = token_type;
					data->type=param_counter;
					data->definovano=true;	
					//musíme nějak aktualizovat type te funkce .... asi nejlepší by bylo kdyby se prostě provadělo při každym načtení řadku v funkci předefinovanu typu funkce	
					htInsert(h_tabulka, k, data );
					result = NO_ERROR;
					return result;
				}else{	
					printf("tato funkce už existuje nemůžete ji redefinovat\n");				
					result = ERROR_2;
					return result;
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
				set_token_and_return();
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

int rule_expr(){
	int result;
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
			if (typ_promene!=TYPE_FLOAT){
				typ_promene=TYPE_INT;		
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
		case TYPE_IDENTIFIER:


		break;
	}
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
			return ret;
		default:
			return NO_ERROR;
	}

}
bool is_operator(){
	return ((token_type >= PLUS && token_type <= MUL)||(token_type >= COMPARE && token_type <= NOTEQUAL));
}
bool is_num(){
	return (token_type == TYPE_INT || token_type == TYPE_FLOAT);
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
