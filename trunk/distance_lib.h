#ifndef DISTANCE_LIB
#define DISTANCE_LIB

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stdlib.h"

const long long max_size = 500;         // max length of strings
const long long N = 50;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

extern "C" int distance(char* filename, char* word, void* results_void);

extern "C" long long get_num_words(char* filename);

extern "C" int get_random_words(char* filename, int num_words, char random_words[num_words][100]);

extern "C" int get_random_words_disjoint(char* filename, int num_words, char random_words[num_words][100], bool upperHalf = false);

extern "C" int prepare_model(char* filename, void* initStruct_void);

extern "C" int distance_no_open(void* initStruct_void, char* word, void* results_void,
                                float threshold);

extern "C" float distance_2words(void* initStruct_void, char* word1, char* word2);

#endif
