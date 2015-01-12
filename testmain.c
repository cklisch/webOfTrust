#include <stdio.h>
#include "trustWeb.h"
#include <stdint.h>

int main(void){
	char* testWeb;
	struct Web web;
	uint8_t trust;
    testWeb = "testWeb.txt";
    printf("%s\n", testWeb);
    web = get_web (testWeb);
	web = mk_randweb (50, 10);
	printf("1\n");
	int i;
	for (i = 1; i <= 20; i++){
		trust = get_trust1 (web, 0, 3, i);
		printf("pathlength %d trust = %d\n", i, trust );
	}
	return 0;
}
