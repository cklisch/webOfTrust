

#include <stdio.h>
#include <stdlib.h>
#include "trustWeb.h"
#include <time.h>
#include <stdint.h>
#include <inttypes.h>

struct Web get_web (char* web_name);
struct Web mk_randweb (int size, int density);
struct Web rand_matrix (int size, int density);
struct Web init_matrix (struct Web web);
void print_matrix (struct Web web);
void mat2web (struct Web *web);
void web2file (char* file_name, struct Web web);
