/*
 * main.c
 *
 *      Author: Doron
 */

#include <stdio.h>
#include <ctype.h>
#include "SPBufferset.h"
char A = 65;
char a = 97;
char ZERO = 48;

int convertCharToDecimalValue(char c){
	/* this method converts a char to number from 0 to 15 */
	int i = 0;
	int j = 0;
	for ( ; i < 10 ; i++ ){
		if (c == ZERO + i ){
			return 0 + i;
		}
	}
	for ( ; j < 6 ; j ++ ){
		if((c == A + j) || (c == a + j)){
			return 10 + j ;
		}
	}
	return -1;
}

int my_pow (int x, int y){
	/* this method returns x^y */
	int retu = 1;
	int i = 0;
	for ( ; i < y ; i ++ ){
		retu = retu * x ;
	}
	return retu;
}

int isNumberLegal(int num, int base){
	/* this method indicates if the number is within it's base */
	if (( num >= 0 ) && (num < base)){
		return 1;
	}
	return 0;
}

int isCharLegal(char c, int base){
	return isNumberLegal(convertCharToDecimalValue(c),base);
}

int convertToBase10(int number[], int base, int length){
	/* convert to base 10
	 * this method assumes that the input is legal
	*/
	int ret = 0;
	int i = length - 1 ;
	for ( ; i >= 0 ; i--){
		ret += number[i]*my_pow(base, length-i-1);
	}
	return ret;
}


int isBaselegal(int base_a) {
	/* this method returns if the base is NOT in range 2-16 */
	return (base_a >= 2) && ( base_a <= 16 );
}

void fromBase10ToBaseB(int num, int base_b){
	int remainder ;
	int length = 0 ;
	static char res[1024];
	char c;
	static char ret[1024];
	int i = 0;
	if (num <= 0){
		printf("The result is: 0\n");

	}
	else{
		while (num > 0){
			remainder = num % base_b;
			if(base_b > 10){
				if(remainder >= 10){
					c = A + (remainder-10);
				}
				else {
					c = ZERO + remainder ;
				}
			}
			else {  /* base<11 */
				c = ZERO + remainder ;
			}
			res[length] = c;
			length++ ;
			num = num / base_b ;
		}
		for ( ; i < length ; i++){
			ret[i] = res[length-1-i];
		}
		printf("The result is: %s\n",ret);
	}
}



int main(){
	int base_a = 0;
	int base_b = 0;
	char F = 0;
	int wordLength = 0;
	int word[1024];
	int baseAToBase10;  /* used in line 131 */
	SP_BUFF_SET();

	printf("Please enter the number's base:\n");
	if (scanf("%d", &base_a)!=1){
		printf("An error occurred!\n");
		return 0;
	}
	if (!isBaselegal(base_a)) {
			printf("Invalid input base\n");
			return 0;
		}

	printf("Please enter the desired base:\n");
	if (scanf("%d", &base_b)!=1){
		printf("An error occurred!\n");
		return 0;
	}
	if(!isBaselegal(base_b)){
			printf("Invalid desired base\n");
			return 0;
		}

	printf("Please enter a number in base %d:\n", base_a);

	F = getchar();
	while(isspace(F)){
		F = getchar();
	}
	while(F != EOF && !isspace(F)){
		if (isCharLegal(F,base_a)){
			/* if the char is in the legal base_a */
			word[wordLength] = convertCharToDecimalValue(F);
			wordLength = wordLength + 1 ;
		}
		else{
		/* the char is not in the base the user chose */
			printf("Invalid number!\n");
			return 0;
		}
		F = getchar();
	}


	baseAToBase10 = convertToBase10(word, base_a, wordLength);

	if (base_b == 10){
		printf("The result is: %d\n", baseAToBase10);
		return 0;
	}

	fromBase10ToBaseB(baseAToBase10, base_b);

	return 0;
}

