#include <stdio.h>

int main(int argc, char const *argv[])
{
	int cont1 = 0;
	int cont0 = 0;
	int i = 32;
	int w = 7;

	printf("Numero dado %d\n", w);

	while (i > 0){
		if(1 & w){
			cont1++;
		}
		w = (w>>1);

		i--;
	}

	cont0 = 32 - cont1; 

	printf("%d\n", cont1);
	printf("%d\n", cont0);
}