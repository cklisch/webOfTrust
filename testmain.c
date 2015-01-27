#include <stdio.h>
#include "trustWeb.h"
#include "web_mgmt.h"
#include <time.h>
#include <assert.h>


int main(void){
        
        char *dir = calloc (30, sizeof (*dir));
        char c, d;
        int pth;
        int size, density;
        clock_t start, stop;
        struct Web web;
        double t;
        printf("would you like to load or create a web\n");
       
        scanf("%c", &c);
        if (c == 'c') {
                printf("size:\n");
                scanf("%d", &size);
                printf("%d\n",size );
                printf("density\n");
                scanf("%d", &density);
                web = rand_matrix (size, density);
                printf("save?");
                scanf("\n%c", &d);
                printf("%c\n",d );
                if (d == 'y') {
                        mat2web (&web);
                        printf("where?\n");
                        scanf("%s", dir);
                        web2file (dir, web);
                }
        }
        else if (c == 'l') {
                printf ("choose your file\n");
                scanf ("%s", dir);
                printf("%s\n", dir );
                web = get_web (dir);
                web2mat (&web);
        }

        printf("print web matrix?\n");
        scanf ("\n%c", &c);
        if (c == 'y') {
                print_matrix (web.matrix, web.size);
        }
        printf("do you want to evaluate the matrix?\n");
        scanf ("\n%c", &c);
        while ( c == 'y') {
                printf("how many threads?\n");
                scanf ("\n%d", &pth);
                assert ((start = clock()) != -1);
                evaluate_web3 (&web, pth);
                stop = clock();
                t = (double)(stop - start) / CLOCKS_PER_SEC;
                printf("%lf\n",t);
                printf("print evaluated matrix?\n");
                scanf ("\n%c", &c);
                if (c == 'y') {
                        print_matrix (web.eval_matrix, web.size);
                }
                printf("again?\n");
                scanf ("\n%c", &c);
        }
        int from_node, to_node;
        printf("do you want to get a trust?\n");
        scanf ("\n%c", &c);

        while (c == 'y') {
                if (c == 'y') {
                        printf("from node:\n");
                        scanf ("\n%d", &from_node);
                        printf("to node:\n");
                        scanf ("\n%d", &to_node);
                        printf("trust %d:\n", web.eval_matrix[from_node][to_node]);
                }
                printf("again?\n");
                scanf ("\n%c", &c);
        }
        return 0;
}
