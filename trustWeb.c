#include <stdio.h>
#include <stdlib.h>
#include "trustWeb.h"
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

void mallocNode (struct Node* retNode, int links) {

	retNode->trusty = malloc ((links + EXTRA) * sizeof(retNode->trusty));
	if (retNode->trusty == NULL){
		free (retNode);
		printf("Error malloc trusty\n");
		return;
	}
	retNode->trust = malloc ((links + EXTRA) * sizeof(retNode->trust));
	if (retNode->trust == NULL){
		free (retNode->trusty);
		free (retNode);
		printf("Error malloc trust\n");
		return;
	}
	retNode->links = links;

	return;

}

 struct Node* getWeb (char* web_name){

	FILE* infile = NULL;
	struct Node* web;
	int nr_nodes, links, id, i=0, j=0;

	infile = fopen(web_name, "r");
	if(infile == NULL){
		printf("Error opening initializing Web\n");
		return NULL;
	}
	
	fscanf(infile,"%d\n", &nr_nodes);

	web = (struct Node*) malloc (nr_nodes * sizeof (struct Node));

	if (web == NULL) {
		printf("Error malloc mallocNode\n");
		return NULL;
	}

	while (i < nr_nodes){
		fscanf (infile,"%d %d", &id, &links);
		if (i != id) {
			printf("getWeb: there is a problem with the input web txt file\n");
			return NULL;
		}
		mallocNode (web, links);
		web->links = links;
		web->ID = id;

		while (j < links){
			fscanf (infile," (%d,%d)", web->trusty++, web->trust++);
			j++;
		}
		web->trusty -= links; web->trust -= links;
		j = 0;
		web++; i++;
	}
	web -= nr_nodes;
	fclose (infile);
	return web;
}

int getWebSize (char* web_name){
	FILE* infile = NULL;
	int nr_nodes;

	infile = fopen(web_name, "r");
	if(infile == NULL){
		printf("Error opening Web for getWebSize\n");
		return 0;
	}

	fscanf(infile,"%d\n", &nr_nodes);
	fclose (infile);
	return nr_nodes;
}

void web2file (char* file_name, struct Node* web, int nr_IDS){
	FILE* outfile = NULL;
	int i = 0, j = 0;

	outfile = fopen (file_name, "w");
	if(outfile == NULL){
		printf("Error opening file to write the web to\n");
		return;
	}

	fprintf(outfile, "%d\n", nr_IDS);

	while (i < nr_IDS){
		fprintf(outfile, "%d %d", web->ID, web->links);
		while (j < web->links){
			fprintf(outfile, " (%d,%d)", web->trusty[j], web->trust[j]);
			j++;
		}
		j = 0;
		fprintf(outfile, "\n");
		web++;
		i++;
	}
	fclose (outfile);
	return;
}

void addNode (char* web_txt){
	FILE* file = NULL;
	int size;

	file = fopen(web_txt, "r+");

	fscanf (file, "%d", &size);
	rewind (file);
	fprintf(file, "%d\n", size + 1);
	fclose (file);

	file = fopen (web_txt, "a");

	fprintf(file, "%d %d\n",size, 0);
	fclose (file);
	return;
}


void addLink (char* web_txt, int ID, int trusty, uint8_t trust){
	struct Node* web;
	int web_size, links, i = 0;

	web = getWeb (web_txt);
	web_size = getWebSize (web_txt);

	if (ID >= web_size) {
		printf("addLink: ID out of scope\n");
		return;
	}
	web += ID;
	links = web->links;

	while (1){
		if (web->trusty[i] == trusty){
			web->trust[i] = trust;
			break;
		}		
		if (i == links) {
			web->trusty[i] = trusty;
			web->trust[i] = trust;
			web->links += 1;
			break;
		}
		i++;
	}

	web -= ID;
	web2file (web_txt, web, web_size);
	free (web);
	return;

}



uint8_t getTrustHelper (struct Node* web, int* path, int toNode){
	struct Node* web_pointer = web;
	int pathlength = path[0];
	if (pathlength == MAX_PATH_LENGTH) return NO_PATH;

	int fromNode = path[pathlength];
	

	web += fromNode;

	int nrOfLinks = web->links;
	if (nrOfLinks == 0) {
		return NO_PATH;
	}	

	uint8_t *trust = NULL ,trustbuffer;
	int i = 0, j, negTrust_acc = 1, initializtion = 0;

	trust = (uint8_t*) malloc (nrOfLinks * sizeof (trust));
	if (trust == NULL){
		printf("getTrustHelper: error malloc trust\n");
		exit(1);

	}

	while (i < nrOfLinks) {
		if (web->trusty[i] == toNode){
			return web->trust[i];
		}
		i++;
	}
	i = 0;
	
	while (i < nrOfLinks) {
		path[0] = pathlength;
		for (j=1; j<path[0]; j++){
			if (web->trusty[i] == path[j]){
				trust[i] = NO_PATH;
				i++;
				j = 0;
				if (i >= nrOfLinks) return NO_PATH;
			}
		}
		path[0] = pathlength + 1;

		if (path[0] == MAX_PATH_LENGTH) return NO_PATH;

		path[path[0]] = web->trusty[i];

		trustbuffer = getTrustHelper (web_pointer, path, toNode);

		if (trustbuffer != NO_PATH) {
			trust[i] = (web->trust[i] * trustbuffer) / MAX_TRUST;
			if (initializtion == 0) {
				negTrust_acc = (MAX_TRUST - trust[i]);
				initializtion = 1;
			}
			else {
				negTrust_acc *= ((MAX_TRUST - trust[i]));
				negTrust_acc /= MAX_TRUST;
			}				
		}
		i++;
	}
	free (trust);
	if (initializtion == 0) return NO_PATH;
	else return (MAX_TRUST - negTrust_acc);
}

uint8_t getTrust (struct Node* web, int fromNode, int toNode){
	int* path = NULL;
	uint8_t trust;

	path = (int*) malloc ((MAX_PATH_LENGTH + 1) * sizeof (path));
	if (path == NULL){
		printf("getTrust: Error malloc\n");
		return 0;
	}
	path[0] = 1;
	path[1] = fromNode;

	trust = getTrustHelper (web, path, toNode);

	return trust;

}

uint8_t newGetTrustHelper (struct TrustAcc* acc, uint8_t* actualTrust, int fromNode) {
	printf("%p\n", actualTrust );
	for (int i = 0; i < 4; ++i)
	{
		printf("%p\n", &actualTrust[i] );
		printf("actualTrust; %d\n", actualTrust[i] );
	}
	if ((*(acc+fromNode)).trust == NO_PATH) {
		printf("in if\n");
		actualTrust[fromNode] = 100;
		return 100;
	}
	printf("1\n");
	if (actualTrust[fromNode] != NO_PATH) return actualTrust[fromNode];
	printf("2\n");
	uint8_t buffer, negTrust_acc = MAX_TRUST;

	struct TrustAcc* buff = (acc+fromNode);


	printf("buff: %p   NULL: %p \n", (*buff).nextAcc, NULL);

	while ((*buff).nextAcc != NULL) {
		printf("00000000000000\n");
		buffer = newGetTrustHelper (acc, actualTrust, (*buff).prevNode);
		negTrust_acc = (negTrust_acc * (MAX_TRUST - buffer)) / MAX_TRUST;
		buff = (*buff).nextAcc;
	}
	
	actualTrust[fromNode] = (MAX_TRUST - negTrust_acc);

	return (MAX_TRUST - negTrust_acc);
}


uint8_t newGetTrust (struct Node* web, int web_size, int fromNode, int toNode) {
	int pathlength = 0, i;
	int* path = NULL;

	path = (int*) malloc (MAX_PATH_LENGTH * sizeof (path));
	if (path == NULL) {
		printf("newGetTrust: mallco error path\n");
		return 0;
	}

	struct TrustAcc* acc = NULL;

	acc = (struct TrustAcc*) malloc (web_size * sizeof (acc));
	if (acc == NULL) {
		printf("newGetTrust: malloc error\n");
		return 0;
	}
	uint8_t* actualTrust = NULL;

	actualTrust = (uint8_t*) malloc (web_size * sizeof (actualTrust));
	printf("%p\n", actualTrust );

	if (actualTrust == NULL) {
		printf("newGetTrust: error malloc actualTrust\n");
		return 0;
	}

	for (i=0; i<web_size; i++){
		(*(acc+i)).trust = NO_PATH;
		(*(acc+i)).nextAcc = NULL;
		(*(acc+i)).prevNode = web_size + 1;
		actualTrust[i] = NO_PATH;
		printf("%p\n", &actualTrust[i] );
		printf("%d\n", actualTrust[i] );
	}
	printf("%p\n", actualTrust );
	printf("%d\n", actualTrust[0] );

	path[0] = fromNode;

	int stepNode, state = 0, breakpoint = 0;

	struct TrustAcc* nextAcc  = NULL, *buffer;
	printf("got to first while loop\n");
	while (1) {
		printf("pathlength %d\n", pathlength );
		if (pathlength == MAX_PATH_LENGTH || path[pathlength] == toNode) {
			printf("in firts if\n");
			pathlength--;
		}
		while ((*(web+path[pathlength])).links <= 0) {
			printf("in second if\n");
			if (pathlength == 0) {
				breakpoint = 1;
				break;
			}
			pathlength--;
		}	
		if (breakpoint == 1) break;

		for (i=0; i<=pathlength; i++) {

		}	

		stepNode = *(*(web+path[pathlength])).trusty++;
		printf("%d\n", stepNode );
		(*(web+path[pathlength])).links--;

		printf("\n");
	
		printf("\n");
		buffer = (acc+stepNode);
		state = 0;
		
		while (1) {	

			if (buffer->trust == NO_PATH){
				buffer->trust    = *(*(web+path[pathlength])).trust++;
				buffer->prevNode = path[pathlength];
				nextAcc = (struct TrustAcc*) malloc (sizeof (nextAcc));
				nextAcc->trust = NO_PATH;
				buffer->nextAcc = nextAcc;
				if (state == 0) {
					pathlength ++;
					path[pathlength] = stepNode;
				}
				break;
			}
			state = 1;
			printf("==============\n");
			buffer = buffer->nextAcc;
		}
		
	}
	printf("got out of first while loop\n");
	printf("%d\n", actualTrust[2] );
	return newGetTrustHelper (acc, actualTrust, toNode);

}



struct Node* randWeb(int size){
	int i = 0, j = 0, links, trust;
	struct Node* web = NULL, *initialpointer;

	web = (struct Node*) malloc (size * sizeof (struct Node));
	initialpointer = web;
	while (i<size){
		srand (time(NULL) * (i+1));
		links = rand() % (20);
		mallocNode (web, links);
		web->links = links;
		web->ID = i;
		while (j<links){	
			srand(time(NULL) * (j+1) * (i+1));		
			web->trusty[j] = rand() % size;
			trust = rand() % 100;
			if (trust<10) trust = 0;
			web->trust[j] = trust;
			j++;
		}
		j = 0;
		web++;
		i++;
	}
	char* randWeb = "randWeb.txt";

	web2file (randWeb, initialpointer, size);

	return initialpointer;
}