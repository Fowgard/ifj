#include "parser.h"

int token_type; //aktualni token
char token_attribute; //atribut aktualniho tokenu
token_t *token;
tStack stack; //Zasobnik pro vyrazy
int brackets_counter; //Leve zavorky ++, prave --
// simulace pravidla <program> -> <st-list>
int program(){
	brackets_counter = 0;
	SInit(&stack);
	int res;

	set_token_and_return();

	switch(token_type){
		//<program> -> <ID> <st-list>	
		case LEFT_BRACKET:
			brackets_counter++;
		case TYPE_INT:
		case TYPE_FLOAT:	
	printf("JSEM V PROGR\n");
			
			while(token_type == TYPE_INT  || token_type == TYPE_FLOAT || 
					(token_type >=PLUS && token_type <= NOTEQUAL)){ //musí se dodělat že tam můžou byt proměné i funkce jako čísla 
				/*if ( funkce,která vrací TYPE_FLOAT/TYPE_FLOAT || proměné TYPE_FLOAT nebo TYPE_INT ){
					nastavíme token_type na to co vrací;
				}*/
				//printf("PUSHUJI: %d\n", token_type);
				SPush(&stack, token_type);
				token_type = set_token_and_return();
		
			}
			if(!is_num(pop_token()) && token_type != RIGHT_BRACKET){
				//ERROOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOR
				printf("Expr nekončí číslem ani závorkou\n");
			}
			res = math_until_EOL();
			if(is_err(res) != NO_ERROR){
				//ZPRACUJ CHYBU UUUUUUUUUUUUUUUUUUUUUUUUUU

				printf("A kurnik\n");
			}else{
				printf("JUpíííííí\n");
			}

		break;

	}
			

}

int set_token_and_return(){
	get_token(&token);
	token_type = &token->type;
	return token_type;
}

int pop_token(){
	token_type = SPop(&stack);
	printf("POP_TOKEN: %d\n",token_type);
	return token_type;
}

int math_until_EOL(){
	int result;
	switch(token_type){
		case PLUS:
		case MINUS:
			if(SEmpty(&stack)){
				result = NO_ERROR;
				return result;
			}
			if(!is_num() || token_type != LEFT_BRACKET){
				result = ERROR_4;
				return result;
			}
			if(result == is_err(math_until_EOL())){
				return result;
			}

		break;

		case DIV:
		case MUL:
			if(SEmpty(&stack)){
				result = ERROR_4;
				return result;
			}
			
			pop_token();
			if(!is_num()){
				result = ERROR_4;
				return result;
			}
			if(result == is_err(math_until_EOL())){
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

			if(result == is_err(math_until_EOL())){
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
