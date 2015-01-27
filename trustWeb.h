#ifndef _TRUSTWEB_
#define _TRUSTWEB_

#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#include "web_mgmt.h"
#include "qsort.h"

#define MAX_TRUST 100
#define NO_PATH (MAX_TRUST + 1)
#define MAX_PATH

#define DEBUG 0
#define NUM_THREADS 2

#define debug_print(fmt, ...) \
            do { if (DEBUG) printf(fmt, __VA_ARGS__); } while (0)


struct Path {
    int length;
    int max;
    int *node;
    int *pos;
};

struct Web {
        int size;
        struct Node *nodes;
        uint8_t **matrix;
        uint8_t **eval_matrix;
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


struct PthEval {
        int id;
        struct Web web;
        uint8_t **eval_matrix;
};

struct Thread {
        int start;
        int stop;
        struct Web web;
};

uint8_t get_trust (struct Web web, int fromNode, int toNode);
void analyse_web (struct Web web);
struct Web evaluate_web (struct Web web);
struct Web evaluate_web2 (struct Web web, int nr_pth);
void evaluate_web3 (struct Web *web, int nr_pth);

#endif
