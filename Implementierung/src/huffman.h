#include <stddef.h>
#include <stdio.h>
#ifndef team_294
#define team_294

//double timee;

//struct MinHNode* root;

struct MinHNode{
  char character;
  unsigned freq;
  struct MinHNode* left;
  struct MinHNode* right;
};

extern struct MinHNode* root;
extern double timee;

struct MinHeap{
  unsigned size;
  unsigned capacity;
  struct MinHNode** array;
};

struct MinHNode* buildHuffmanTree(char character[], int freq[], int size);

char* huffman_encode_tests(size_t len, const char data[len]);

void huffman_decode_tests(size_t len, const char data[len]);

void huffman_encode(size_t len, const char data[len], FILE* outputFile, int to_print);

void huffman_decode(size_t len, const char data[len], FILE* outputFile, int to_print);

void huffman_encode_2_tests(size_t len, const char data[len]);

void huffman_encode_2(size_t len, const char data[len], FILE* outputFile, int to_print);

#endif
