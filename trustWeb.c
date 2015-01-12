#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "trustWeb.h"
#include <time.h>
#include <stdint.h>

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
        int i, j;
        struct Web web;
        struct Node *node;
        FILE *infile;
        infile = fopen (web_name, "r");
        if (infile == NULL) {
                printf("get_web: Error opening %s\n", web_name);
                return web;
        }
        
        fscanf(infile,"%d\n", &(web.size));
        node = (struct Node*) malloc (web.size * sizeof (*node));
        if (node == NULL) {
                printf("get_web: Error malloc_node\n");
                return web;
        }
        printf("%d\n",web.size );        
        
        for (i = 0; i < web.size; i++){
                fscanf (infile,"%d", &node[i].links);
                malloc_node (&node[i], node[i].links);
                for (j = 0; j < node[i].links; j++){
                        fscanf (infile," (%d,%"SCNd8")",
                        &node[i].trusty[j], &node[i].trust[j]);
                }
        }
        web.nodes = node;
        fclose (infile);
        return web;
}



uint8_t getTrustHelper (struct Node *node, int from_node, int to_node, struct Path *path)
{
        
        if (path->length == path->max) 
            return 0;
        int i;
        for (i = 0; i < path->length; i++) 
            if (path->pos[i] == from_node) 
                return 0;

        if (to_node == from_node) 
            return MAX_TRUST;

        
        path->pos[path->length] = from_node;
        int pathlength = ++path->length;
        uint8_t trust, distrust_acc = MAX_TRUST;      


        for (i = 0; i < node[from_node].links; i++) {

                trust = getTrustHelper (node, node[from_node].trusty[i], to_node, path);

                path->length = pathlength;
                
                trust = ((node[from_node].trust[i] * trust) / MAX_TRUST) ; 
                if ((distrust_acc * (MAX_TRUST - trust)) % MAX_TRUST > 0) {
                    distrust_acc = (distrust_acc * (MAX_TRUST - trust)) / MAX_TRUST;
                    distrust_acc++;
                }
                else distrust_acc = (distrust_acc * (MAX_TRUST - trust)) / MAX_TRUST;

                                                                    
                
                
        }
                
        return (MAX_TRUST - distrust_acc);
}

uint8_t get_trust1 (struct Web web, int from_node, int to_node, int max_pathlength)
{
        uint8_t trust;
        int *pos = (int*) calloc (max_pathlength , sizeof (*pos));
        if (pos == NULL) {
                printf("getTrust: Error malloc\n");
                return 0;
        }
        struct Path path;
        path.pos = pos;
        path.max = max_pathlength;
        path.length = 0;
        trust = getTrustHelper (web.nodes, from_node, to_node, &path);
        free (pos);
        return trust;

}

uint8_t newGetTrustHelper (struct TrustAcc* acc, uint8_t* actual_trust, int from_node, int to_node, int *path) 
{       
        printf("fromNode %d pathlength %d\n",from_node, path[0]); 
        if (from_node == to_node) return 100;
        if (actual_trust[from_node] != NO_PATH) return actual_trust[from_node];
        int pathlength = path[0], is_cyclic = 0, i;
        path[path[0]++] = from_node;
        
        uint8_t trust, neg_trust = MAX_TRUST;
        struct TrustAcc* cur_acc = (acc + from_node);
        while (cur_acc->nextAcc != NULL) {
                for (i = 1; i < pathlength; i++){                        
                        if (path[i] == cur_acc->prevNode) {
                                printf("is_cyclic %d\n", i );
                                is_cyclic = 1;
                                cur_acc = cur_acc->nextAcc;
                                if (cur_acc->nextAcc == NULL) return 0;
                        }
                }
                trust = newGetTrustHelper (acc, actual_trust, (*cur_acc).prevNode, to_node, path );
                path[0] = pathlength;
                trust = (trust * cur_acc->trust) / MAX_TRUST;
                neg_trust = (neg_trust * (MAX_TRUST - trust)) / MAX_TRUST;
                cur_acc = (*cur_acc).nextAcc;
        }        
        if (is_cyclic == 0) actual_trust[from_node] = (MAX_TRUST - neg_trust);
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
        for (i = 0; i < n; i++){
                acc[i].trust = NO_PATH;
                (*(acc+i)).nextAcc = NULL;
                (*(acc+i)).prevNode = web.size + 1;
        }
        return acc;
}

uint8_t get_trust2 (struct Web web, int fromNode, int toNode, int max_pathlength)
{
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
        int pathlength = 0;

        int acc_used = 0, next_node;
        struct Node current_node;
        struct TrustAcc *new_acc  = malloc_TrustAcc (1, web), *next_node_acc;
        
        while (1) {
                current_node = web.nodes[path[pathlength]];
                while (link_cnt[path[pathlength]] >= current_node.links) {
                        if (pathlength == 0) {
                                goto recursion;
                        }
                        current_node = web.nodes[path[--pathlength]];
                }  
                if (pathlength == max_pathlength || path[pathlength] == toNode) {

                        current_node = web.nodes[path[--pathlength]];
                }
                
                next_node = current_node.trusty[link_cnt[path[pathlength]]];
                link_cnt[path[pathlength]]++;
                next_node_acc = (acc + next_node);
                acc_used = 0;
                while (1) {     
                        if (next_node_acc->nextAcc == NULL){
                                next_node_acc->trust = current_node.trust[link_cnt[path[pathlength]]-1];
                                next_node_acc->prevNode = path[pathlength];
                                new_acc = malloc_TrustAcc (1, web);
                                next_node_acc->nextAcc = new_acc;
                                if (acc_used == 0) {
                                        path[++pathlength] = next_node;
                                }
                                break;
                        }
                        acc_used = 1;
                        next_node_acc = next_node_acc->nextAcc;
                        
                }
        }
recursion:
        path[0] = 1;
        printf("recursion\n");
        return newGetTrustHelper (acc, actualTrust, toNode, fromNode, path);
}

void web2file (char* file_name, struct Web web)
{
       int i = 0, j = 0;
       FILE* outfile = outfile = fopen (file_name, "w");     
       if(outfile == NULL){
               printf("Error opening file to write the web to\n");
               return;
       }
       fprintf(outfile, "%d\n", web.size);
       for (i = 0; i < web.size; i++){
               fprintf(outfile, "%d", web.nodes[i].links);
               for (j = 0; j < web.nodes[i].links; j++){
                       fprintf(outfile, " (%d,%d)", web.nodes[i].trusty[j], web.nodes[i].trust[j]);
               }
               fprintf(outfile, "\n");
       }
       fclose (outfile);
       return;
}



struct Web mk_randweb(int size, int density){
        int i, j, k, links, trusty, seed = 1;
        struct Node *node = (struct Node*) malloc (size * sizeof (struct Node));
        for (i = 0; i < size; i++){
                srand (time(NULL) * seed++);
                links = (rand() % size) % density;
                malloc_node (&node[i], links);
                node[i].links = links;
                for (j = 0; j < links; j++){
                        srand(time(NULL) + seed++);
                        trusty = rand() % size;              
                        for (k = 0; k < j; k++) {
                                if (node[i].trusty[k] == trusty) {
                                        srand(time(NULL) + (i+1) * seed++);
                                        k = 0;
                                        trusty = rand() % size;
                                }
                        }
                        node[i].trusty[j] = trusty;
                        node[i].trust[j] = (rand() % MAX_TRUST) ;
                }
        }

        struct Web web;
        web.size = size;
        web.nodes = node;
        web2file ("randWeb.txt", web);
        return web;
}
