#include "web_mgmt.h"
#include "trustWeb.h"
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
