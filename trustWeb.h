#ifndef _TRUSTWEB_
#define _TRUSTWEB_

#include <stdint.h>

#define MAX_TRUST 100
#define NO_PATH 0

struct Path {
    int length;
    int max;
    int *pos;
};

struct Web {
        int size;
        struct Node *nodes;
};

struct Node {
        int links;
        int *trusty;
        uint8_t *trust; 
};


struct TrustAcc {
        int prevNode;
        uint8_t trust; 
        struct TrustAcc* nextAcc;
};


uint8_t get_trust1 (struct Web web, int fromNode, int toNode, int max_pathlength);
uint8_t get_trust2 (struct Web web, int fromNode, int toNode, int max_pathlength);



#endif
