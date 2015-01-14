#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "trustWeb.h"
#include <time.h>
#include <stdint.h>






uint8_t getTrustHelper (struct Node *node, int from_node, int to_node, struct Path *path, uint8_t *actual_trust)
{   
        printf("from_node %d\n", from_node );
        if (path->length == path->max) 
            return 0;
        int i;
        for (i = 0; i < path->length; i++) 
            if (path->pos[i] == from_node) 
                return 0;

        if (from_node == to_node) 
            return MAX_TRUST;
        printf("%d\n", actual_trust[from_node] );
        if (actual_trust[from_node] != MAX_TRUST + 1)
            return actual_trust[from_node];


        path->pos[path->length] = from_node;
        int pathlength = ++path->length;
        uint8_t trust, distrust_acc = MAX_TRUST;      

        for (i = 0; i < node[from_node].links; i++) {

                trust = getTrustHelper (node, node[from_node].trusty[i], to_node, path, actual_trust);

                path->length = pathlength;
                
                trust = ((node[from_node].trust[i] * trust) / MAX_TRUST) ; 
                if ((distrust_acc * (MAX_TRUST - trust)) % MAX_TRUST > 0) {
                    distrust_acc = (distrust_acc * (MAX_TRUST - trust)) / MAX_TRUST;
                    distrust_acc++;
                }
                else distrust_acc = (distrust_acc * (MAX_TRUST - trust)) / MAX_TRUST;        
        }
        actual_trust[from_node] = (MAX_TRUST - distrust_acc);
        printf("%d actual_trust %d\n", from_node, actual_trust[from_node]);
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
        uint8_t *actual_trust = (uint8_t*) malloc (web.size * sizeof (*actual_trust));
        int i;
        for (i = 0; i < web.size; i++) {
            actual_trust[i] = MAX_TRUST + 1;
        }
        struct Path path;
        path.pos = pos;
        path.max = max_pathlength;
        path.length = 0;
        trust = getTrustHelper (web.nodes, from_node, to_node, &path, actual_trust);
        free (pos);
        return trust;

}

uint8_t newGetTrustHelper (struct TrustAcc* acc, uint8_t* actual_trust, int from_node, int to_node, struct Path *path) 
{       
        // printf("from_node %d pathlength %d actual_trust %d\n", from_node, path->length, actual_trust[from_node]);
        if (from_node == to_node)
            return 100;
        if (actual_trust[from_node] != NO_PATH) 
            return actual_trust[from_node];
        int is_cyclic = 0, i;
        path->pos[path->length] = from_node;
        int pathlength = path->length;
        uint8_t trust, distrust = MAX_TRUST;
        struct TrustAcc* cur_acc = (acc + from_node);
        while (cur_acc->nextAcc != NULL) {
                for (i = 0; i < path->length; i++){                        
                        if (path->pos[i] == cur_acc->prevNode) {
                                is_cyclic = 1;

                                cur_acc = cur_acc->nextAcc;
                                if (cur_acc->nextAcc == NULL) 
                                    goto end;
                                i = 0;
                        }
                }
                path->length++;
                trust = newGetTrustHelper (acc, actual_trust, cur_acc->prevNode, to_node, path );
                path->length = pathlength;
                trust = (trust * cur_acc->trust) / MAX_TRUST;
                distrust = (distrust * (MAX_TRUST - trust)) / MAX_TRUST;
                cur_acc = (*cur_acc).nextAcc;
        }
end:
        if (is_cyclic == 0) actual_trust[from_node] = (MAX_TRUST - distrust);
        // printf("from_node %d actual_trust%d\n", from_node, actual_trust[from_node] );
        return (MAX_TRUST - distrust);
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
                (*(acc+i)).prevNode = web.size;
        }
        return acc;
}

uint8_t get_trust2 (struct Web web, int from_node, int to_node, int max_pathlength)
{
        int *path = (int*) calloc (max_pathlength , sizeof (*path));
        if (path == NULL) {
                printf("newGetTrust: mallco error path\n");
                return 0;
        }
        struct TrustAcc *acc = malloc_TrustAcc (web.size, web);
        uint8_t *actual_trust = (uint8_t*) calloc (web.size , sizeof (*actual_trust));
        if (actual_trust == NULL) {
                printf("newGetTrust: calloc error actualTrust\n");
                return 0;
        }
        int *link_cnt = (int*) calloc (web.size, sizeof (*link_cnt));
        if (link_cnt == NULL) {
                printf ("newGetTrust: malloc error link_cnt");
                return 0;
        }
        int pathlength = 0;

        int acc_used = 0, next, curr;
        struct TrustAcc *new_acc  = malloc_TrustAcc (1, web), *next_acc;
        path[0] = from_node;
        struct Node *node = web.nodes; 
        struct Path *pathx = (struct Path*) malloc (sizeof (*pathx));
        while (1) {
                curr = path[pathlength];
                while (link_cnt[curr] >= node[curr].links) {
                        if (pathlength == 0) {
                                goto recursion;
                        }
                        curr = path[--pathlength];
                }  
                if (pathlength == max_pathlength || curr == to_node) {
                        curr = path[--pathlength];
                }
                
                next = node[curr].trusty[link_cnt[curr]];
                acc_used = 0;
                next_acc = acc + next;
                while (1) {     
                        if (next_acc->nextAcc == NULL){
                                next_acc->trust = node[curr].trust[link_cnt[curr]];
                                next_acc->prevNode = curr;
                                new_acc = malloc_TrustAcc (1, web);
                                next_acc->nextAcc = new_acc;
                                if (acc_used == 0) {
                                        path[++pathlength] = next;
                                }
                                link_cnt[curr]++;
                                break;
                        }
                        acc_used = 1;
                        next_acc = next_acc->nextAcc;
                        
                }
        }
recursion:
        pathx->pos = path;
        pathx->length = 0;
        pathx->max = max_pathlength;
        return newGetTrustHelper (acc, actual_trust, to_node, from_node, pathx);
}



