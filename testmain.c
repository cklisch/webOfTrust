#include <stdio.h>
#include "trustWeb.h"
#include "web_mgmt.h"
#include <time.h>
#include <assert.h>


int main(void){
        clock_t start, stop;
        double tm = 0.0;

        struct Web web = rand_matrix (200, 20);

        assert ((start = clock()) != -1);
        struct Web e_web = evaluate_web (web);
        stop = clock();
        tm = (double) (stop-start)/CLOCKS_PER_SEC;
        printf("Run time: %f\n", tm);
        tm = 0.0;

   
        assert ((start = clock()) != -1);
        struct Web e_web2 = evaluate_web2 (web, 8);
        stop = clock();
        tm = (double) (stop-start)/CLOCKS_PER_SEC;
        printf("Run time: %f\n", tm);
        tm = 0.0;


        assert ((start = clock ()) != -1);
        struct Web e_web3 = evaluate_web3 (web, 8);
        stop = clock();
        tm = (double) (stop-start)/CLOCKS_PER_SEC;
        printf("Run time: %f\n", tm);
        return 0;
}
