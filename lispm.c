#include <stdio.h>
#include <stdlib.h>
#include<string.h>


#define heapSize 1000 
#define _REDTEXT_ "\033[31m"
#define _GREENTEXT_ "\033[32m"
#define _NORMALTEXT_ "\033[0m"

// snke_case
// CamlCase
// dromedaryCase - pascalCase

enum type {INTEGER, SYMBOL, LIST};




struct lispList{
	union element{
		int num;
		char symbol; 
		struct lispList* list;
	}  info;
	 
	enum type t; 
	struct lispList* next;
	//int num; 
};

struct tokenizeList{
	char* info;
	struct tokenizeList* next;
};


struct lispList heap[heapSize];


struct lispList* freeList;
/////////////////////////////////////////////////Strings////////////////////////////////////////////////////////////

int isNumber(char* string){ // Функция isNumber() проверяет является ли строка числом 
	int stringLen = strlen(string);
	int i = 0;
	if(string[0] == '-'){
		i = 1;
	}
	for(; i < stringLen; i++){
		if(string[i] < '0' || string[i] > '9')
			return 0;
	}
	return 1;
}

int strToInt(char *string){
	if (*string == '\0' ){
		return 0;
	}
	char *endptr;
	int num = strtol(string, &endptr, 10);
	return num;
}
/*Функция slicesStr() вырезает из строки символы с 
аданным диапазоном индексов и возвращает полученную подстроку.*/

char* slicesStr (char* str, int start, int end){
	if(start > end){
		printf("start > end\n");
		exit(1);
	}
	int len = end - start;
	
	char* res = malloc(sizeof(char) * (len+1));
	for(int g = 0; start < end; start++, g++){
		res[g] = str[start];
	}
	res[len] ='\0';

	return res;
} 


char* charToString (char symbol){ //Функция charToString() преобразует символ в строку.
  	 char* res = malloc(sizeof(char)*2);
  	 res[0] = symbol;
  	 res[1] = '\0';
     return res;	
}

int eqStrings(char* string1, char* string2){
	return !strcmp(string1, string2);
}
//////////////////////////////////////////////////////List////////////////////////////////////////////////////////////

struct lispList* reverseLispList(struct lispList* list){ //Функция reverseLispList() изменяет порядок элементов списка на обратный.
	struct lispList* res = NULL;
	for(; list != NULL; ){
		struct lispList* listSave = list->next;
	 	list->next = res; 
	 	res = list;
	 	list = listSave;
	}
	return res;	
}


struct tokenizeList* reverseTokenizeList(struct tokenizeList* list){ // Функция reverseTokenizeList() изменяет порядок элементов списка на обратный.
	struct tokenizeList* res = NULL;
	for(; list != NULL; ){
		struct tokenizeList* listSave = list->next;
	 	list->next = res; 
	 	res = list;
	 	list = listSave;
	}
	return res;
}

struct lispList* cutOutList(struct lispList* startPoint, struct lispList*  endPoint){ // обе переменные хронят в себе элементы которые не должны не находится внутри нового листа
	struct lispList* resList = startPoint;
	struct lispList* searchElement = startPoint;
	if(searchElement == endPoint){
		searchElement->next = NULL;
		return NULL;
	}
	while(searchElement->next != endPoint){ //->next
		searchElement = searchElement->next;
	}
	searchElement->next = NULL;
	return resList;
}
/////////////////////////////////////////////////Initialize List///////////////////////////////////////////////////////

struct lispList* initializeFreeList(){
	
	for(int i = 0; i < heapSize - 2 ; i ++){
		//heap[i].element = NULL;
		heap[i].next = heap + i + 1;
	}
	heap[heapSize - 1].next = NULL;
	
	return heap;
}

/*
Функция initializeFreeList() инициализирует список свободных ячеек
памяти из массива heap, который выступает в роли хранилища элементов 
типа lispList. Она заполняет next каждой ячейки так, чтобы они указывали 
на следующую свободную ячейку памяти. Возвращаемое значение - 
указатель на первый элемент массива.
*/

/////////////////////////////////////////////////Cast Elemnt//////////////////////////////////////////////////////////

struct lispList* consInteger( int num, struct lispList* pointer){ 	   // Cоздает ячейку типа INTEGER
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.num = num;
	cell->t = INTEGER;
	cell->next = pointer;
	
	return cell;	
}
struct lispList* consSymbol( char symbol, struct lispList* pointer){ 	   // Cоздает ячейку типа INTEGER
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.symbol = symbol;
	cell->t = SYMBOL;
	cell->next = pointer;
	
	return cell;	
}
struct lispList* consList( struct lispList* info, struct lispList* pointer){// Cоздает ячейку типа LIST
	struct lispList* cell = freeList;
	freeList = freeList->next;
		
	cell->info.list = info;
	cell->t = LIST;
	cell->next = pointer;
	
	return cell;	
}
/*
Функции consInteger() и consList() создают новую ячейку памяти, заполняют
ее переданными параметрами и возвращают указатель на нее. 
*/

/////////////////////////////////////////////////Print//////////////////////////////////////////////////////////


void printError(const char* text, const char* function);
void printList(struct lispList* list);

void printElement(union element info, enum type t){ 
	if( t == INTEGER){
		printf("%d ", info.num );
	}else if(t == LIST){
		printf("[ ");
		printList( info.list );
		printf("] ");
	}else if(t == SYMBOL){
		printf("'%c'", info.symbol);
	}
	else{
		printError("Wrong type", __FUNCTION__);
	}	
	
}


void printList(struct lispList* list){
	for(; list != NULL; list = list->next){
		printElement(list->info, list->t);
	}
			
}

void printTokenizeList(struct tokenizeList* list){
	for(; list != NULL; list = list->next){
		printf("'%s' ", list->info);
	}
	printf("\n");
}


/*
Функция printList() выводит на экран значения всех элементов типа
lispList списка, переданного в качестве параметра. Она проходит по 
всем элементам, вызывая функцию printElement() для каждого из них.
*/




void printError(const char* text, const char* function){
	printf(_REDTEXT_);
	printf("\nError: ");
	printf(_NORMALTEXT_);
	printf("'%s' in function ", text);
	printf(_GREENTEXT_);
	printf("%s\n\n", function);
	printf(_NORMALTEXT_);
	exit(1);
	
}

/////////////////////////////////////////////////Tokenize//////////////////////////////////////////////////////////


struct tokenizeList*  addString (char* data, struct tokenizeList* ls){ //Функция addString() создает новый элемент типа tokenizeList.

	struct tokenizeList* res = malloc( sizeof(struct tokenizeList)); 
	res->info = data;
	res->next = ls;
	return res;
}


struct tokenizeList* reverseTokenizeList(struct tokenizeList* list); // Функция reverseTokenizeList() изменяет порядок элементов списка на обратный.


/*Функция slicesStr() вырезает из строки символы с 
аданным диапазоном индексов и возвращает полученную подстроку.*/

char* slicesStr (char* str, int start, int end);


//Функция charToString() преобразует символ в строку.
char* charToString (char symbol); 



/*Функция printTokenizeList() выводит на экран значения 
всех элементов типа tokenizeList переданного списка.*/

void printTokenizeList(struct tokenizeList* list);



void freeTokenizeList(struct tokenizeList* list){ //Функция freeTokenizeList() очищает все елементв списка типа tokenizeList
	struct tokenizeList* listSave;
	while( list != NULL){
		listSave = list;
		list = list->next;
		free(listSave->info);
		free(listSave);
	}
}



struct tokenizeList*  tokenize(char* str){
	int len = strlen(str);
	int start = 0;
	int i = 0;
	int addhappened = 1; 
	struct tokenizeList* ls = NULL;
	
	for(; i < len; i ++){
		if((str[i] == ' '|| str[i] == '\n' || str[i] == '\t') && !addhappened){ // добавление информации в результат 
			ls = addString(slicesStr(str, start, i), ls); //вырезание нужной информации в результат 
			start = i + 1 ;
			addhappened = 1;
		}
		else if ( i + 1 < len && str[i + 1] == '(' ){ 	// нахождение  скобок
		    if (!addhappened){
		    	ls = addString(slicesStr(str, start, i), ls);  //вырезание предыдущей информации в результат 
		    }
		    char* res = malloc(sizeof(char) * 3);
		    res[0] = str[i];
		    res[1] = str[i + 1];
		    res[2] = '\0';
			ls = addString(res, ls);
			start = i + 2;
			i ++;
			addhappened = 1;
		}
		else if (( str[i] == '(' || str[i] == ')')){ // нахождение квадратных скобок
		    if (!addhappened){
		    	ls = addString(slicesStr(str, start, i), ls);  //вырезание предыдущей информации в результат 
		    }
			ls = addString(charToString(str[i]), ls);
			start = i + 1;
			addhappened = 1;
		}
		else if ((str[i] == ' '|| str[i] == '\n' || str[i] == '\t') && addhappened){//пропуск лишних пробелов и перезодных строк 
			start = i + 1;
		}
		else{
			addhappened = 0;
		}
	} 
	if(i != start){
		ls = addString(slicesStr(str, start, i), ls );
	}
	//free(str);
	return reverseTokenizeList(ls); //reverslist(reverselist_copy
} 



/*
Функции printTokenizeList(), addString(), reverseTokenizeList(), slicesStr()
и charToString() отвечают за токенизацию входной строки. 
*/




/////////////////////////////////////////////////Define Type///////////////////////////////////////////////////////
int isNumber(char* string);


struct lispList* defineType(char* info,struct lispList* stack){
		if(isNumber(info)){
			int num = strToInt(info);
			free(info);
			return consInteger(num , stack );
		}
		else if( eqStrings(info,"[") || eqStrings(info,"]") ){
			return consSymbol(info[0], stack );
		}
		else{
			printError("Type can not be defined", __FUNCTION__);
		}
		
}




/////////////////////////////////////////////////Parsing///////////////////////////////////////////////////////

struct lispList* parsing (struct tokenizeList* list){

	struct lispList* stack = NULL;

	while (list != NULL){
		char* elemnt = list->info;
		if( eqStrings(elemnt, "]")){ // добавление чисел и строк на стек 
			//printf("aaaaaaa\n");
			stack = defineType(elemnt, stack);
		} 
		else if( eqStrings(elemnt, "]")){  //добавление списков на стек
			printList(stack);
			printf("\n");
			exit(1);
			free(list->info);
			struct lispList* res;
			
			if ( stack->t == SYMBOL && stack->info.symbol == '['){
				//stack = free_first_stack(stack);
				stack = stack->next;
				res = NULL;		
			} 
			else{
				struct lispList* list_start = stack;
	           	struct lispList* list_end = stack; 
	           	
	           	int a = 0;
				
				while (  stack->t != SYMBOL )  {
					a++;
					list_end = list_end->next;
				}
				res = cutOutList(list_start, list_end);
				//stack = free_first_stack(list_end);
				stack = stack->next;
				
			}
			stack = consList( reverseLispList(res) ,stack);
		
		}
		//ls = free_first_stack_shallow(ls);//
		
	}
	
	return stack;
}




/////////////////////////////////////////////////Main//////////////////////////////////////////////////////////

int main(){

	freeList = initializeFreeList();
	struct tokenizeList* stringList =  tokenize("(1 2 5)");
	printTokenizeList(stringList);
	struct lispList* res = parsing(stringList);
	printList(res);
	printf(" - res\n");
	//freeTokenizeList(stringList);
	exit(1);
	
	struct lispList*  list = NULL;
	struct lispList*  list2 = NULL;
	list = consInteger( 14, list);
	list = consInteger( 1, list);
	list = consInteger( 2, list);
	list = consInteger( 3, list);

	list2 = consInteger( 4, list2);
	list2 = consInteger( 5, list2);

	list = consList(list2, list);
	printf("\n");
	printList(list);
	printf("\n"); 	

}
