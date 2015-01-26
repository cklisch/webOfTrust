
#include "trustWeb.h"


void analysis_report (int *node_analysis, int * nodes, int size)
{
        FILE *report = fopen ("report.txt", "w");
        if (report == NULL) {
                printf("analysis_report: open file Error\n");
                return;
        }
        int i;
        for (i = 0; i < size; i++) {
                fprintf(report, "%4d %d\n", nodes[i], node_analysis[i]);
        }
}

void analyse_web (struct Web web)
{
        int *node_analysis = (int*) calloc (web.size, sizeof (*node_analysis));
        if (node_analysis == NULL) {
                printf("web_analysis: malloc Error\n");
                return;
        }
        int *id = (int*) malloc (web.size * sizeof (*id));
        if (id == NULL) {
                printf("web_analysis: malloc Error\n");
                return;
        }
        int i, j;
        for (i = 0; i < web.size; i++) {
                id[i] = i;
                for (j = 0; j < web.size; j++) {
                        node_analysis[j] += web.matrix[i][j];
                }
        }
        printf("loop done\n");
        my_qsort (node_analysis, id, web.size);
        printf("qsort done\n");
        analysis_report (node_analysis, id, web.size);
        return;


}






void recursiv_step (uint8_t trust, uint8_t *eval, uint8_t *path, int from_node, struct Web web)
{
        uint8_t new_trust, old_trust;
        int i;
        for (i = 0; i < web.size; i++) {
                if (web.matrix[from_node][i] != 0 && path[i] == 0) {
                        debug_print("%d", i);
                        old_trust = eval[i];
                        new_trust = (trust * web.matrix[from_node][i]) / MAX_TRUST;
                        eval[i] = MAX_TRUST - ((MAX_TRUST - new_trust) * (MAX_TRUST - old_trust)) / MAX_TRUST;
                        path[i] = eval[i] - old_trust;
                        if (path[i] != 0)
                                recursiv_step (path[i], eval, path, i, web);
                        path[i] = 0;
                }
        }
}



uint8_t* eval_node (int id, struct Web web)
{
        uint8_t *eval = (uint8_t*) calloc (web.size, sizeof (*eval));
        uint8_t *path = (uint8_t*) calloc (web.size, sizeof (*eval));

        path[id] = 100;

        recursiv_step (100, eval, path, id, web);

        free (path);
        return eval;
        
}


struct Web evaluate_web (struct Web web)
{       

        int i;
        uint8_t **eval_matrix = (uint8_t**) malloc (web.size * sizeof (eval_matrix));
        for (i = 0; i < web.size; i++){
                eval_matrix[i] = eval_node (i, web);
        }
        struct Web e_web;
        e_web.size = web.size;
        e_web.nodes = web.nodes;
        e_web.matrix = eval_matrix;

        return e_web;
}


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


void *eval_node2 (void *args)
{        
        struct PthEval *str = (struct PthEval *) args;
        uint8_t *path = (uint8_t*) calloc (str->web.size, sizeof (*path));

        path[str->id] = 100;

        recursiv_step (100, str->eval_matrix[str->id], path, str->id, str->web);

        free (path);
        pthread_exit((void*) &str->id);
        
}


struct Web evaluate_web2 (struct Web web, int nr_threads)
{       

        int i, t;
        uint8_t **eval_matrix = (uint8_t**) malloc (web.size * sizeof (eval_matrix));
        pthread_t pth[nr_threads];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        void *status;
        struct PthEval str[web.size];
       
        for (i = 0; i < web.size;){                
                for (t = 0; t < nr_threads && i < web.size; t++, i++) {
                        eval_matrix[i] = (uint8_t*) calloc (web.size, sizeof (*eval_matrix));
                        str[i].web = web;
                        str[i].eval_matrix = eval_matrix;
                        str[i].id = i;
                        pthread_create(&pth[t], &attr, eval_node2, (void *) &str[i]);
                }
                for (t = 0; t < nr_threads; t++) {
                        pthread_join (pth[t], &status);

                }
        }
        struct Web e_web;
        e_web.size = web.size;
        e_web.nodes = web.nodes;
        e_web.matrix = eval_matrix;

        return e_web;
}

void *eval_thread (void *args)
{
        struct Thread *str = (struct Thread *) args;

        int i;

        for (i = str->start; i < str->stop; i++) {
                str->web.eval_matrix[i] = eval_node (i, str->web);
        }

        pthread_exit((void*) &str->start);


}

struct Web evaluate_web3 (struct Web web, int nr_pth)
{
        int i, t;
        uint8_t **eval_matrix = (uint8_t**) malloc (web.size * sizeof (eval_matrix));
        pthread_t pth[nr_pth];
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        void *status;
        struct Thread str[nr_pth];
        web.eval_matrix = eval_matrix;
        printf("done\n");



        for (i = 0; i < nr_pth; i++) {
                str[i].web = web;
                str[i].start = i * (web.size / nr_pth);
                printf("start %d : %d\n",i, str[i].start);
                str[i].stop = (i + 1) * (web.size / nr_pth);
                printf("stop %d : %d\n",i, str[i].stop);

                pthread_create (&pth[i], &attr, eval_thread, (void *) &str[i]);
        }
        for (i = 0; i < nr_pth; i++) {
                pthread_join (pth[i], &status);
        }

        struct Web e_web;
        e_web.size = web.size;
        e_web.nodes = web.nodes;
        e_web.matrix = eval_matrix;

        return e_web;
}
