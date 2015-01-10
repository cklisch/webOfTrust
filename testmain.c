#include <stdio.h>
#include "trustWeb.h"
#include <stdint.h>

int main(void){
	char* testWeb;
        printf("wtf");
	struct Web web;
	uint8_t trust;
        printf("wtf");
        testWeb = "testWeb.txt";
	web = get_web(testWeb);	
        printf("wtf");
	trust = get_trust1 (web, 0, 1, 8);


	printf("trust = %d\n", trust );


	return 0;
}
