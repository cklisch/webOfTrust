#ifndef _TRUSTWEB_
#define _TRUSTWEB_

#include <stdint.h>

#define MAX_LINKS 1000
#define EXTRA 1             /*extra space to add new links in active web*/
#define MAX_PATH_LENGTH 8
#define MAX_TRUST 100
#define NO_PATH (MAX_TRUST + 1)

struct Node {
	int ID;
	int links;
	uint8_t *trusty;
	int *trust;	
};


struct TrustAcc {
	int prevNode;
	uint8_t trust; 
	struct TrustAcc* nextAcc;
};

struct Node* getWeb(char* web_name);
void web2file (char* file_name, struct Node* web, int nr_IDS);
int getWebSize (char* web_name);
void addNode (char* web_txt);
void addLink (char* web_txt, int ID, int trusty, uint8_t trust);
uint8_t getTrust (struct Node* web, int fromNode, int toNode);
struct Node* randWeb(int size);
uint8_t newGetTrust (struct Node* web, int web_size, int fromNode, int toNode);


#endif