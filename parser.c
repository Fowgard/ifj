#include "parser.h"

int token_type; //aktualni token
char token_attribute; //atribut aktualniho tokenu
token_t *token;

// simulace pravidla <program> -> <st-list>
int program(){

	int res;

	printf("JSEM V PROGR\n");
	SET_TOKEN();
	switch(token_type){
		//<program> -> <ID> <st-list>
		case TYPE_INT:
		case TYPE_FLOAT:
			res = math_until_EOL();
			if(is_err(res) != NO_ERROR){
				//ZPRACUJ CHYBU UUUUUUUUUUUUUUUUUUUUUUUUUU

				printf("A kurnik\n");
			}else{
				printf("JUpíííííí\n");
			}

	}
			

}

int SET_TOKEN(){
	printf("SET TOKEN ZACATEK\n");
	get_token(&token);
	printf("SET TOKEN U KONCE\n");
	token_type = &token->type;
	printf("TOKEN TYPE: %d\n", token_type);
	return token_type;
}

int math_until_EOL(){

	int result;
	switch(token_type){
		case PLUS:
		case MINUS:
		case DIV:
		case MUL:
			if(SET_TOKEN() == END_OF_LINE){
				if(!is_num()){
					result = ERROR_4;
					return result;
				}else{
					result = NO_ERROR;
					return result;
				}
			}
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
			if(SET_TOKEN() == END_OF_LINE){
				result = NO_ERROR;
				return result;
			}

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
