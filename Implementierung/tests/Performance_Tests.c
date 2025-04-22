//#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../src/huffman.h"


#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_run_test(test) do { char *message = test(); tests_run++; if (message) return message; } while (0)

struct MinHNode* root;
double timee; 

int number_of_runs;

int tests_run = 0;

const char* strings[] = {
      "<r8<}).q<r-`22/SvS6^eUn Xar5S1+HUGtv!3gx(:Db\"Nrfbs~*u7bR[HJe$I\\&K\\ZX=U#V;[P$Ux.\"]e)C*&rJr_DFzhvm0; d"};





static char* testing_encode_V0(){

    int runs = number_of_runs;
    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    while(runs > 0) {
            huffman_encode_tests(strlen(strings[0]), strings[0]);
            runs--;
    }
    
    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);
    double time = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );

int length = sizeof(strings) / sizeof(strings[0]);

printf("Encode V0 was called %i times with a List of %d elements. \n", number_of_runs, length);
printf("Encode V0: Total Excution Time: %.9f\n\n",time);

return 0;


}

static char* testing_encode_V1(){

    int runs = number_of_runs;
    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    while(runs > 0) {

        huffman_encode_2_tests(strlen(strings[0]), strings[0]);
        runs--;
    }

    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);
    double time = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );

int length = sizeof(strings) / sizeof(strings[0]);

printf("Encode V1 was called %i times with a List of %d elements. \n", number_of_runs, length);
printf("Encdeo V1: Total Excution Time: %.9f\n\n",time);

return 0;


}

static char* Average_Time_encode_V0(){

double total_time = 0;
int runs = number_of_runs;

    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    while(runs > 0) {

            huffman_encode_tests(strlen(strings[0]), strings[0]);
            runs--;
    }

    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);
    double time = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );

    total_time += time;


double average_time = total_time / number_of_runs;
printf("Average Execution Time Encode V0: %.9f\n", average_time);
return 0;

}

static char* Average_Time_encode_V1(){

double total_time = 0;
int runs = number_of_runs;

    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    while(runs > 0) {

            huffman_encode_2_tests(strlen(strings[0]), strings[0]);
            runs--;
    }

    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);
    double time = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );

    total_time += time;


double average_time = total_time / number_of_runs;
printf("Average Execution Time Encode V1: %.9f\n", average_time);
return 0;

}


////we need to test the performance of huffman_encode_file_input //////





static char* all_tests() {
    mu_run_test(testing_encode_V0);
    mu_run_test(testing_encode_V1);
    mu_run_test(Average_Time_encode_V0);
    mu_run_test(Average_Time_encode_V1);
    
    return 0;
}


int main(int argc, char* argv[]){

    if (argc == 1 || argc > 2) {
            number_of_runs = 200;
    }
    else {
            number_of_runs = atoi(argv[1]);
    }

    char *result = all_tests();
    if (result == 0) {
        printf("ALL TESTS PASSED\n");
    } else {
        printf("TESTS FAILED: %s\n", result);
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
