#include <stdio.h>
#include "trustWeb.h"
#include "web_mgmt.h"
#include <stdint.h>

int main(void){
	char* testWeb;
	struct Web web;
	uint8_t trust;
    testWeb = "testWeb.txt";
    printf("%s\n", testWeb);
    // web = mk_randweb (1000, 10);
     web = get_web (testWeb);



	printf("1\n");
	int i;
	for (i = 1; i <= 20; i++){
		trust = get_trust1 (web, 0, 4, i);
		printf("1> pathlength %d trust = %d\n", i, trust );
		trust = get_trust2 (web, 0, 4, i);
		printf("2> pathlength %d trust = %d\n\n", i, trust );
	}
	return 0;
}
