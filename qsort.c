#include <stdio.h>
#include <stdlib.h>

/**
 * Sorts an array of inting point numbers in ascending order.
 * An iterative implementation of a recursive algorithm
 *
 * @param
 *     numbers    - array of inting point numbers
 *     nr         - length of numbers array
**/
void my_qsort (int *numbers, int *indices, int nr) {

        int i, j, start, temp_i;
        int avg, temp;

  /* requires additional space to keep segment positions */
        int spos = 0;
        int *stack = NULL;

        if (nr<2)
                return;

        stack = (int *) malloc(sizeof(int)*nr);  /* on the save side */
        
        if (stack==NULL){
    
                printf("Error: requested memory could not be allocated");
                return;
        }

  /* initialize stack + starting position */
        start = 0;
        stack[0] = nr;
        spos = 1;


  /** main-loop: sort as long as there are segment positions in stack **/
        while (spos>0) {

    /* get starting/ending indices of leftest segment */
                i = start;
                j = stack[spos-1]-1;


    /* segment length <2  =>  already sorted  =>  work on neighbour segment */
                if (i>=j) {
                        start = stack[spos-1];
                        spos--;
                        continue;
                }
    /* partioning of one segment into two segments */
                avg = .5f*numbers[(i+j+1)>>1] + .25f*(numbers[i] + numbers[j]);
                while (1) {
                        while (i<j && numbers[i]>=avg)
                                i++;
                        while (i<=j && numbers[j]<avg)
                                j--;
                        if (i>=j){
                                break;
                        }
                        temp = numbers[i];
                        temp_i = indices[i];
                        numbers[i] = numbers[j];
                        indices[i] = indices[j];
                        numbers[j] = temp;
                        indices[j] = temp_i;
                        i++;
                        j--;
                }

    /* add new segment position to stack */
                stack[spos] = i;
                spos++;
        }

}  /* iterative_sort */