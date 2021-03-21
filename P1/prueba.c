#include <stdio.h>
int main()
{	
	int n1 = 1;
	int n2 = 6;
	int res = n1^n2;
	printf("%d\n",res);
	if (res == 1 || res == 2 || res == 4) {
		printf("Cix");	
	} else {
		printf("No");
	}

}
