#include <stdio.h>
#include "trustWeb.h"
#include <stdint.h>

int main(void){
	char* testWeb;
	struct Web web;
	uint8_t trust;
    testWeb = "testWeb.txt";
    printf("%s\n", testWeb);
	web = get_web(testWeb);	
	trust = get_trust1 (web, 0, 2, 8);


	printf("trust = %d\n", trust );


	return 0;
}
