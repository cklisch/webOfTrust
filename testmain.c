#include <stdio.h>
#include "trustWeb.h"
#include <stdint.h>

int main(void){
	char* testWeb;
	struct Node* web;
	int web_size, i = 0;
	uint8_t x = 2, trust;

	testWeb = "testWeb.txt";

	web = getWeb (testWeb);

	trust = newGetTrust (web, 4, 0 , 3);


	printf("trust = %d\n", trust );


	return 1;
}