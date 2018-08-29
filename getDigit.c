#include <stdio.h>

//global variables

unsigned int digit1;
unsigned int digit2;
unsigned int digit3;
unsigned int digit4;

void getDigit (int number){
	int i;

	for(i = 9; i > 0; i--){
		if(i * 1000 <= number){
			digit1 = i;
			break;
		}
	}

	number -= i * 1000;

	for(i = 9; i > 0; i--){
		if (i * 100 <= number){
			digit2 = i;
			break;
		}
	}

	number -= i * 100;

	for(i = 9; i > 0; i--){
		if (i * 10 <= number){
			digit3 = i;
			break;
		}
	}

	number -= i * 10;

	digit4 = number;
}

int main(){

	unsigned int number;

	printf("Digite o numero em milhar que deseja obter os digitos \n");

	scanf("%d", &number);

	getDigit(number);

	printf("Os digitos foram %d %d %d %d \n", digit1, digit2, digit3, digit4);
	
	return 0;
}