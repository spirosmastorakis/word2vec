//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "stdlib.h"

#include <algorithm>
#include <random>
#include <numeric>
#include <iostream>

const long long max_size = 500;         // max length of strings
const long long N = 50;                  // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries

struct initStruct {
  long long size;
  char* vocab;
  float *M;
  long long words;
  char *bestw[N];
};

struct result {
  char resultValue[100];
};

struct resultFormat {
  int nResultsToReturn;
  result resultsArray[5];
};

long long get_num_words(char* filename)
{
  FILE *f;
  char file_name[max_size];
  long long words, size;
  strcpy(file_name, filename);
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  return words;
}

int get_random_words(char* filename, int num_words, char random_words[num_words][100]) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  char ch;
  float *M;
  char *vocab;
  //if (argc < 2) {
  //  printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
  //  return 0;
  //}
  strcpy(file_name, filename);
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  srand(time(NULL));   // should only be called once
  int random_num = 0;
  for (int i = 0; i < num_words; i++) {
    random_num = rand() % words;
    strcpy(random_words[i], &vocab[random_num * max_w]);
  }
  return 0;
}

int prepare_model(char* filename, void* initStruct_void) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  char ch;
  float *M;
  char *vocab;
  struct initStruct* mystruct = (struct initStruct*) initStruct_void;
  //for (a = 0; a < N; a++) mystruct->bestw[a] = bestw[a];
  //if (argc < 2) {
  //  printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
  //  return 0;
  //}
  strcpy(file_name, filename);
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &(mystruct->words));
  fscanf(f, "%lld", &(mystruct->size));
  mystruct->vocab = (char *)malloc((long long)mystruct->words * max_w * sizeof(char));
  for (a = 0; a < N; a++) mystruct->bestw[a] = (char *)malloc(max_size * sizeof(char));
  mystruct->M = (float *)malloc((long long)mystruct->words * (long long)mystruct->size * sizeof(float));
  vocab = mystruct->vocab;
  M = mystruct->M;
  words = mystruct->words;
  size = mystruct->size;
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  return 0;
}

float distance_2words(void* initStruct_void, char* word1, char* word2)
{
  struct initStruct* mystruct = (struct initStruct*) initStruct_void;
  float product = 0;
  float magnitude = 0;
  float square1 = 0;
  float square2 = 0;
  float dist = 0;
  char st1[max_size];
  char st[100][max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  float *M;
  char *vocab;
  long long pos1 = 0;
  long long pos2 = 0;
  words = mystruct->words;
  size = mystruct->size;
  vocab = mystruct->vocab;
  M = mystruct->M;

  // First word
  strcpy(st1, word1);
  cn = 0;
  b = 0;
  c = 0;
  while (1) {
    st[cn][b] = st1[c];
    b++;
    c++;
    st[cn][b] = 0;
    if (st1[c] == 0) break;
    if (st1[c] == ' ') {
      cn++;
      b = 0;
      c++;
    }
  }
  cn++;
  for (a = 0; a < cn; a++) {
    for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
    if (b == words) b = -1;
    bi[a] = b;
    pos1 = bi[a];
    //printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
    if (b == -1) {
      //printf("Out of dictionary word!\n");
      return -1;
    }
  }

  // Second word
  strcpy(st1, word2);
  cn = 0;
  b = 0;
  c = 0;
  while (1) {
    st[cn][b] = st1[c];
    b++;
    c++;
    st[cn][b] = 0;
    if (st1[c] == 0) break;
    if (st1[c] == ' ') {
      cn++;
      b = 0;
      c++;
    }
  }
  cn++;
  for (a = 0; a < cn; a++) {
    for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
    if (b == words) b = -1;
    bi[a] = b;
    pos2 = bi[a];
    //printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
    if (b == -1) {
      //printf("Out of dictionary word!\n");
      return -1;
    }
  }

  for (a = 0; a < size; a++) {
    dist += M[a + pos1 * size] * M[a + pos2 * size];
    // square1 += M[a + pos1 * size] * M[a + pos1 * size];
    // square2 += M[a + pos2 * size] * M[a + pos2 * size];
    // printf("First: %f\n", M[a + pos1 * size]);
    // printf("Second: %f\n", M[a + pos2 * size]);
  }
  // magnitude = sqrt(square1) * sqrt(square2);
  // printf("Magnitude %f\n", magnitude);
  // printf("Product %f\n", product);
  // dist = 1 - (product / magnitude);
  //printf("Distance: %f\n", dist);
  return dist;
}


float distance(void* initStruct_void, char* word) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  char ch;
  float *M;
  char *vocab;
  struct initStruct* mystruct = (struct initStruct*) initStruct_void;
  //resultFormat* results = (resultFormat*) results_void;
  words = mystruct->words;
  size = mystruct->size;
  vocab = mystruct->vocab;
  M = mystruct->M;
  for (a = 0; a < N; a++) bestw[a] = mystruct->bestw[a];
  printf ("Word %s\n", word);
  //if (argc < 2) {
  //  printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
  //  return 0;
  //}
  // strcpy(file_name, filename);
  // f = fopen(file_name, "rb");
  // if (f == NULL) {
  //   printf("Input file not found\n");
  //   return -1;
  // }
  // fscanf(f, "%lld", &words);
  // fscanf(f, "%lld", &size);
  // vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  //for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  // M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  // if (M == NULL) {
  //   printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
  //   return -1;
  // }
  // for (b = 0; b < words; b++) {
  //   a = 0;
  //   while (1) {
  //     vocab[b * max_w + a] = fgetc(f);
  //     if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
  //     if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
  //   }
  //   vocab[b * max_w + a] = 0;
  //   for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
  //   len = 0;
  //   for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
  //   len = sqrt(len);
  //   for (a = 0; a < size; a++) M[a + b * size] /= len;
  // }
  // fclose(f);
  //while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    //printf("Enter word or sentence (EXIT to break): ");
    a = 0;
    // while (1) {
    //   st1[a] = fgetc(stdin);
    //   if ((st1[a] == '\n') || (a >= max_size - 1)) {
    //     st1[a] = 0;
    //     break;
    //   }
    //   a++;
    // }
    strcpy(st1, word);
    if (!strcmp(st1, "EXIT")) return 0;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        printf("Out of dictionary word!\n");
        return -1;
      }
    }
    if (b == -1) return 0;
    printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
          }
          bestd[a] = dist;
          strcpy(bestw[a], &vocab[c * max_w]);
          break;
        }
      }
    }
    // for (a = 0; a < results->nResultsToReturn; a++) strcpy(results->resultsArray[a].resultValue, bestw[a]);
    //strcpy(result, bestw[0]);
    for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
  //}
  return 0;
}

int distance_no_open(void* initStruct_void, char* word, void* results_void,
                     float threshold) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  int random_number, counter, above_threshold = 0;
  char ch;
  float *M;
  char *vocab;
  struct initStruct* mystruct = (struct initStruct*) initStruct_void;
  resultFormat* results = (resultFormat*) results_void;
  words = mystruct->words;
  size = mystruct->size;
  vocab = mystruct->vocab;
  M = mystruct->M;
  srand (time(NULL));
  for (a = 0; a < N; a++) bestw[a] = mystruct->bestw[a];
  // printf ("Word %s\n", word);
  //while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    //printf("Enter word or sentence (EXIT to break): ");
    a = 0;
    // while (1) {
    //   st1[a] = fgetc(stdin);
    //   if ((st1[a] == '\n') || (a >= max_size - 1)) {
    //     st1[a] = 0;
    //     break;
    //   }
    //   a++;
    // }
    strcpy(st1, word);
    if (!strcmp(st1, "EXIT")) return 0;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      //printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        //printf("Out of dictionary word!\n");
        return -1;
      }
    }
    if (b == -1) return 0;
    //printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
          }
          bestd[a] = dist;
          if (bestd[a] >= threshold) {
            strcpy(bestw[a], &vocab[c * max_w]);
            above_threshold++;
          }
          break;
        }
      }
    }
    if (above_threshold <= results->nResultsToReturn) {
      for (a = 0; a < above_threshold; a++)
        strcpy(results->resultsArray[a].resultValue, bestw[a]);
      return above_threshold;
    }
    std::vector<int> num_vec;

    if (above_threshold > N)
      above_threshold = N;

    for (int i = 0; i < above_threshold; i++) {
      num_vec.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(num_vec.begin(), num_vec.end(), g);

    for (a = 0; a < results->nResultsToReturn; a++)
      strcpy(results->resultsArray[a].resultValue, bestw[num_vec[a]]);
    // //strcpy(result, bestw[0]);
    // for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
  //}
  return results->nResultsToReturn;
}

int main(int argc, char **argv) {
  FILE *f;
  char st1[max_size];
  char *bestw[N];
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, a, b, c, d, cn, bi[100];
  char ch;
  float *M;
  char *vocab;
  if (argc < 2) {
    printf("Usage: ./distance <FILE>\nwhere FILE contains word projections in the BINARY FORMAT\n");
    return 0;
  }
  strcpy(file_name, argv[1]);
  struct initStruct mystruct;
  prepare_model(file_name, (void*) &mystruct);
  struct timeval t1,t0;
  gettimeofday(&t0, 0);
  printf("Distance %f\n", distance_2words((void*) &mystruct, argv[2], argv[3]));
  //resultFormat results;
  //results.nResultsToReturn = 5;
  //distance_no_open((void*) &mystruct, argv[2], (void*) &results, 0.4);
  gettimeofday(&t1, 0);
  // double dif = double( (t1.tv_usec-t0.tv_usec) / 1000);
  // printf ("Elasped time is %lf milliseconds.\n", dif);
  double dif = double( (t1.tv_usec-t0.tv_usec));
  printf ("Elasped time is %lf microseconds.\n", dif);
  //return 0;
  // printf ("Back \n");
  // distance((void*) &mystruct, argv[2]);
  //return 0;
  // printf("Number of words in corpus: %lld\n", get_num_words(file_name));
  // return 0;
  // char random_words[1000][100];
  // get_random_words(file_name, 1000, random_words);
  // for (int i = 0; i < 1000; i++)
  //   printf("Word %d is: %s\n", i, random_words[i]);
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words);
  fscanf(f, "%lld", &size);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char));
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  while (1) {
    for (a = 0; a < N; a++) bestd[a] = 0;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    printf("Enter word or sentence (EXIT to break): ");
    a = 0;
    while (1) {
      st1[a] = fgetc(stdin);
      if ((st1[a] == '\n') || (a >= max_size - 1)) {
        st1[a] = 0;
        break;
      }
      a++;
    }
    if (!strcmp(st1, "EXIT")) break;
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
      printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        printf("Out of dictionary word!\n");
        break;
      }
    }
    if (b == -1) continue;
    printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
          }
          bestd[a] = dist;
          strcpy(bestw[a], &vocab[c * max_w]);
          break;
        }
      }
    }
    for (a = 0; a < N; a++) printf("%50s\t\t%f\n", bestw[a], bestd[a]);
  }
  return 0;
}
