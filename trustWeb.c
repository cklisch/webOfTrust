
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "trustWeb.h"
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

void malloc_node (struct Node* node, int links)
{
        node->trusty = malloc ((links) * sizeof (node->trusty));
        if (node->trusty == NULL){
                free (node);
                printf("malloc_node: Error malloc trusty\n");
                return;
        }
        node->trust = malloc ((links) * sizeof (node->trust));
        if (node->trust == NULL){
                free (node->trusty);
                free (node);
                printf("malloc_node: Error malloc trust\n");
                return;
        }
        node->links = links;
        return;
}

struct Web get_web (char *web_name)
{
        int links, i=0, j=0;
        struct Web web;
        struct Node *node;
        FILE *infile = fopen(web_name, "r");
        if(infile == NULL){
                printf("get_web: Error opening %s\n", web_name);
                return web;
        }
        
        printf("wtf");
        fscanf(infile,"%d\n", &(web.size));
        printf("wtf");
        node = (struct Node*) malloc (web.size * sizeof (struct Node));
        if (node == NULL) {
                printf("get_web: Error malloc_node\n");
                return web;
        }
        
        malloc_node (node, web.size );
        for (i = 0; i < web.size; i++){
                fscanf (infile,"%d", &node[i].links);
                for (j = 0; j < node[i].links; j++){
                        fscanf (infile," (%d,%"SCNd8")",
                        &node[i].trusty[j], &node[i].trust[j]);
                }
        }
        web.nodes = node;
        fclose (infile);
        return web;
}



uint8_t getTrustHelper (struct Node* node, int* path, int toNode, int max_pathlength)
{
        int pathlength = path[0];
        struct Node* first_node = node;
        if (pathlength == max_pathlength) goto no_path;
        int fromNode = path[pathlength];
        node += fromNode;
        int nrOfLinks = node->links;
        if (nrOfLinks == 0) goto no_path; 

        uint8_t *trust = NULL ,trustnext_node_acc;
        int i = 0, j, negTrust_acc = 1, initializtion = 0;

        trust = (uint8_t*) malloc (nrOfLinks * sizeof (trust));
        if (trust == NULL){
                printf("getTrustHelper: error malloc trust\n");
                exit(1);

        }
        for (i = 0; i < nrOfLinks; i++) {
                if (node->trusty[i] == toNode){
                        return node->trust[i];
                }
        }
        for (i = 0; i < nrOfLinks; i++) {
                for (j=1; j<path[0]; j++){
                        if (node->trusty[i] == path[j]){
                                trust[i] = NO_PATH;
                                i++;
                                j = 0;
                                if (i >= nrOfLinks) goto no_path;
                        }
                }
                path[0]++;

                if (path[0] == max_pathlength) goto no_path;

                path[path[0]] = node->trusty[i];

                trustnext_node_acc = getTrustHelper (first_node, path, toNode, max_pathlength);

                if (trustnext_node_acc != NO_PATH) {
                        trust[i] = (node->trust[i] * trustnext_node_acc) / MAX_TRUST;
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
        if (initializtion == 0) {
no_path:                
                return NO_PATH;
        }
        else return (MAX_TRUST - negTrust_acc);
}

uint8_t get_trust1 (struct Web web, int fromNode, int toNode, int max_pathlength)
{
        uint8_t trust;
        int *path = (int*) malloc ((max_pathlength) * sizeof (*path));
        if (path == NULL) {
                printf("getTrust: Error malloc\n");
                return 0;
        }
        path[0] = 0;
        path[1] = fromNode;

        trust = getTrustHelper (web.nodes, path, toNode, max_pathlength);

        return trust;

}

uint8_t newGetTrustHelper (struct TrustAcc* acc, uint8_t* actual_trust, int from_node, int *path) 
{
        path[path[0]] = from_node;
        path[0]++;

        if ((*(acc + from_node)).nextAcc == NULL) {
                printf("in if\n");
                actual_trust[from_node] = 100;
                return 100;
        }
        printf("1\n");
        if (actual_trust[from_node] != NO_PATH) return actual_trust[from_node];
        
        int i;
        for (i = 1; i <= path[path[0]]; i++){
                if (path[i] == from_node) return 0;
        }

        printf("2\n");
        uint8_t trust, neg_trust = MAX_TRUST;

        struct TrustAcc* cur_acc = (acc + from_node);

        while ((*cur_acc).nextAcc != NULL) {
                printf("00000000000000\n");
                trust = newGetTrustHelper (acc, actual_trust, (*cur_acc).prevNode, path );
                neg_trust = (neg_trust * (MAX_TRUST - trust)) / MAX_TRUST;
                cur_acc = (*cur_acc).nextAcc;
        }
        
        actual_trust[from_node] = (MAX_TRUST - neg_trust);

        return (MAX_TRUST - neg_trust);
}

struct TrustAcc* malloc_TrustAcc (int n, struct Web web)
{
        struct TrustAcc *acc = (struct TrustAcc*) malloc (n * sizeof (*acc));
        if (acc == NULL) {
                printf("malloc error TrustAcc\n");
                return NULL;
        }
        int i;
        for (i = 0; i < web.size; i++){
                acc[i].trust = NO_PATH;
                (*(acc+i)).nextAcc = NULL;
                (*(acc+i)).prevNode = web.size + 1;
        }
        return acc;
}

uint8_t newGetTrust (struct Web web, int fromNode, int toNode, int max_pathlength)
{
        int pathlength = 0, i;
        int *path = (int*) malloc ((max_pathlength + 1) * sizeof (*path));
        if (path == NULL) {
                printf("newGetTrust: mallco error path\n");
                return 0;
        }
        struct TrustAcc *acc = malloc_TrustAcc (web.size, web);

        uint8_t *actualTrust = (uint8_t*) calloc (web.size, sizeof (*actualTrust));
        if (actualTrust == NULL) {
                printf("newGetTrust: calloc error actualTrust\n");
                return 0;
        }
        path[0] = fromNode;
        int *link_cnt = (int*) calloc (web.size, sizeof (*link_cnt));
        if (link_cnt == NULL) {
                printf ("newGetTrust: malloc error link_cnt");
                return 0;
        }
        int acc_used = 0, next_node;
        struct Node current_node;
        struct TrustAcc *new_acc  = malloc_TrustAcc (1, web), *next_node_acc;
        printf("got to first while loop\n");
        while (1) {
                current_node = web.nodes[path[pathlength]];
                printf("pathlength %d\n", pathlength );
                if (pathlength == max_pathlength || path[pathlength] == toNode) {
                        printf("in firts if\n");
                        pathlength--;
                }
                while (link_cnt[path[pathlength]] == current_node.links) {
                        printf("in second if\n");
                        if (pathlength == 0) {
                                goto recursion;
                        }
                        pathlength--;
                }       
                next_node = *current_node.trusty++;
                link_cnt[path[pathlength]]++;
                next_node_acc = (acc + next_node);
                acc_used = 0;
                
                while (1) {     

                        if (next_node_acc->trust == NO_PATH){
                                next_node_acc->trust = *current_node.trust++;
                                next_node_acc->prevNode = path[pathlength];
                                next_node_acc->nextAcc = new_acc;
                                if (acc_used == 0) {
                                        pathlength ++;
                                        path[pathlength] = next_node;
                                }
                                break;
                        }
                        acc_used = 1;
                        printf("==============\n");
                        next_node_acc = next_node_acc->nextAcc;
                }
                
        }
recursion:
        path[0] = 0;
        return newGetTrustHelper (acc, actualTrust, toNode, path);

}



struct Web mk_randweb(int size, int density){
        int i = 0, j = 0, links, trust;
        struct Node *nodes = (struct Node*) malloc (size * sizeof (struct Node));
        struct Node *ptr = nodes;
        for (i = 0;i < size; i++){
                srand (time(NULL) * (i+1));
                links = rand() % density;
                malloc_node (nodes, links);
                nodes->links = links;
                for (j = 0; j<links; j++){        
                        srand(time(NULL) * (j+1) * (i+1));              
                        nodes->trusty[j] = rand() % size;
                        nodes->trust[j] = (rand() % 99) + 1;
                }
                nodes++;
        }
        struct Web web;
        web.size = size;
        web.nodes = ptr;
        return web;
}
