#include <stdio.h>
#include "trustWeb.h"
#include <stdint.h>

int main(void){
	char* testWeb;
	struct Web web;
	uint8_t trust;
    testWeb = "testWeb.txt";
    printf("%s\n", testWeb);
	web = mk_randweb (5000, 15);
	printf("1\n");
	trust = get_trust1 (web, 0, 2, 5);
	printf("__trust = %d\n", trust );
	trust = get_trust1 (web, 0, 2, 6);
	printf("__trust = %d\n", trust );
	trust = get_trust1 (web, 0, 2, 7);
	printf("__trust = %d\n", trust );



	return 0;
}
