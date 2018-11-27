#include "parser.h"

int token_type; //aktualni token
char *token_attribute; //atribut aktualniho tokenu
token_t *token = NULL;
tStack stack; //Zasobnik pro vyrazy
tHTable* h_tabulka; // Hashovaci tabulka
int brackets_counter; //Leve zavorky ++, prave --
int param_counter; //pocitadlo parametru pri definici funkce

bool already_init=false;//podmínka pro inicializaci parseru

//int result; // Pouzivame pro ulozeni navratove hodnoty z funkce (Error nebo ne error :D )
// simulace pravidla <program> -> <st-list>
int program(){
	if(!already_init)
		init_parser();

	int res;

	set_token_and_return();

	switch(token_type){
		//<program> -> <ID> <st-list>	
		case LEFT_BRACKET:
			brackets_counter++;
		case TYPE_INT:
		case TYPE_FLOAT:	
			
			while(token_type == TYPE_INT  || token_type == TYPE_FLOAT || 
					(token_type >=PLUS && token_type <= NOTEQUAL)){ //musí se dodělat že tam můžou byt proměné i funkce jako čísla 
				/*if ( funkce,která vrací TYPE_FLOAT/TYPE_FLOAT || proměné TYPE_FLOAT nebo TYPE_INT ){
					nastavíme token_type na to co vrací;
				}*/
				//printf("PUSHUJI: %d\n", token_type);
				SPush(&stack, token_type);
				token_type = set_token_and_return();
		
			}
			//printf("KONEC PUSHOVANI\n----------------------------:\n");
			if(!is_num(pop_token()) && token_type != RIGHT_BRACKET){
				printf("Expr nekončí číslem ani závorkou\n");
				return ERROR_2;
			}
			res = rule_expr();

			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}else{
				call_generator(res);
			}

		break;

		case DEF:	
			rule_def();
			res = rule_expr();
			printf("jsem error ? %d\n", res);
			if(is_err(res) != NO_ERROR){
				call_generator(res);
				return res;
			}else{
				call_generator(res);
			}

		case END_OF_FILE:	
			return NO_ERROR;

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
				if(htRead(&h_tabulka,k) == NULL){//Pokud nebyla nalezena polozka, tak ji vloz
					
					if(result != is_err(rule_def())){
						return result;
					}
					tData data; //data pro hashovací tabulku
					data.type=param_counter;	
					htInsert(&h_tabulka, k, data );
					result = NO_ERROR;
					printf("vracom no ERROR\n");
					return result;

				}else{					
					result = ERROR_2;
					return result;
				}
			}
		break;
		case TYPE_IDENTIFIER:
			printf("stav zavorek %d\n", brackets_counter);
			if (brackets_counter==0){
				if(set_token_and_return() != LEFT_BRACKET){
					result = ERROR_2;
					return result;
				}
				else{
					brackets_counter++;
					if(result != is_err(rule_def())){
						return result;
					}
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
				if(result != is_err(rule_def())){
					return result;
				}

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
					if(result != is_err(rule_def())){
						return result;
					}

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
				set_token_and_return();
				if(result != is_err(rule_stat())){
					return result;
				}

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
				if(result != is_err(rule_def())){
					return result;
				}
			}
			else{
				result = ERROR_2;
				return result;
			}

		break;



	}


}

int rule_stat(){
	switch(token_type){
		case END:
			return NO_ERROR;


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
			if(result == is_err(rule_expr())){
				return result;
			}

		break;

		case DIV:
		case MUL:
		case COMPARE:
		case LOE:
		case LESSTHAN:
		case MOE:
		case MORETHAN:
		case NOTEQUAL:
			if(SEmpty(&stack)){
				result = ERROR_4;
				return result;
			}
			
			pop_token();

			if(!is_num()){
				result = ERROR_4;
				return result;
			}
			if(result == is_err(rule_expr())){
				return result;
			}

		break;
		case TYPE_INT:
		case TYPE_FLOAT:
			if(SEmpty(&stack)){
				result = NO_ERROR;
				return result;
			}

			pop_token();

			if(!is_operand()){
				result = ERROR_4;
				return result;
			}

			if(result == is_err(rule_expr())){
				return result;
			}
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
bool is_operand(){
	return ((token_type >= PLUS && token_type <= MUL)||(token_type >= COMPARE && token_type <= NOTEQUAL));
}
bool is_num(){
	return (token_type == TYPE_INT || token_type == TYPE_FLOAT);
}

void init_parser(){

	token = malloc(sizeof(token_t));
	brackets_counter = 0;
	SInit(&stack);
	htInit(&h_tabulka);
	already_init=true;

}