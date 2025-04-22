#include "huffman.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>


/////used for the  V2 of Encode/Decode/buildDict
struct Pair{
    char character;
    char* binCode;
};

size_t newLen;
//The dictionary is a global variable so that it can be accessed from encode and decode
struct Pair dictionary[256];


// create new nodes
struct MinHNode* newNode(char character, unsigned freq){
  struct MinHNode* temp = (struct MinHNode*)malloc(sizeof(struct MinHNode)); //
  temp->left = NULL;
  temp->right = NULL;
  temp->character = character;
  temp->freq = freq;
  return temp;
}

// create an empty min heap
struct MinHeap* allocate_space_MinHeap(unsigned capacity){
  struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
  minHeap->size = 0;
  minHeap->capacity = capacity;
  minHeap->array = (struct MinHNode**)malloc(minHeap->capacity * sizeof(struct MinHNode*));
  return minHeap;
}

// swap two nodes
void swap_nodes(struct MinHNode** a, struct MinHNode** b){
  struct MinHNode* t = *a;
  *a = *b;
  *b = t;
}

// here we rearrange the Heap so that the to preserve the Heap Invariant H[i] ≤ min {H[2i + 1], H[2i + 2]}
// where H[2i+1] and H[2i+2] are the children of H[i]
void sift_down_up(struct MinHeap* minHeap, int start_index){
  int smallest = start_index;
  unsigned left = 2 * start_index + 1;
  unsigned right = 2 * start_index + 2;
  // check if the left child has smaller frequency, if so smallest = left
  if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq){
    smallest = left;
  }
  // check if the right child has smaller frequency, if so smallest = right
  if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq){
    smallest = right;
  }
  //if the start_index is not equal to the index of the smallest element we swap them and call sift_down_up
  if (smallest != start_index){
    swap_nodes(&minHeap->array[smallest], &minHeap->array[start_index]);
    sift_down_up(minHeap, smallest);
  }
}

//extract min
struct MinHNode* pop_Min(struct MinHeap* minHeap){
  //get the first element in temp
  struct MinHNode* temp = minHeap->array[0];
  //replace the first element with the last element
  minHeap->array[0] = minHeap->array[minHeap->size - 1];

  --minHeap->size;
  //sift_down_up will be applied to the first element (as it got assigned the last element)
  sift_down_up(minHeap, 0);

  return temp;
}

//insert the given node to the given Heap
void insert(struct MinHeap* minHeap, struct MinHNode* minHeapNode){
  //place the new node at the end of the array
  minHeap->array[minHeap->size] = minHeapNode;
  ++minHeap->size;

  //call heapify to restore the heap property
  sift_down_up(minHeap, (minHeap->size - 1));
}

void set_up_MinHeap(struct MinHeap* minHeap){
  int n = minHeap->size - 1;
  int i;
  //example array: [A:5 ; B:3 ; D:1 ; K:1 ; R:2]
  // size = 5 -> n = 4
  //The function calculates the index of the last non-leaf node in the array,
  //which is in our case (4 - 1) / 2 = 1 (that's B:3). Then it calls sift_down_up on each node from that index (1) down to index 0
  for (i = (n - 1) / 2; i >= 0; i--) {
    sift_down_up(minHeap, i);
  }
}
//initializes the MinHeap with the characters and their frequencies and then calls set_up_MinHeap to rearrange them to create the MinHeap invariant
struct MinHeap* create_MinHeap(char character[], int freq[], int size){
  struct MinHeap* minHeap = allocate_space_MinHeap(size);
  for (int i = 0; i < size; i++){
    minHeap->array[i] = newNode(character[i], freq[i]);
  }
  minHeap->size = size;
  set_up_MinHeap(minHeap);

  return minHeap;
}

struct MinHNode* buildHuffmanTree(char character[], int freq[], int size){
  struct MinHNode* left;
  struct MinHNode* right;
  struct MinHNode* start_index;
  struct MinHeap* minHeap = create_MinHeap(character, freq, size);
  while (minHeap->size != 1){
    left = pop_Min(minHeap);
    right = pop_Min(minHeap);

    start_index = newNode('&', left->freq + right->freq);
    start_index->left = left;
    start_index->right = right;


    insert(minHeap, start_index);
  }

  return pop_Min(minHeap);
}

char** buildDict(struct MinHNode* root, char* code, char** dict){
  
  if (root == NULL){
    return dict;
  }
  // if root is a leaf we add the symbol and the code to the dictionary
  if (!(root->left) && !(root->right)){
    // assign the code to the respective character in the dictionary using their ASCII value as an index in the dictionary array.
    // all the characters that are not in the Huffman Tree will have 0 assigned to their value in the dictionary (from the initialization)
    dict[(unsigned char)root->character] = code;
    return dict;
  }

  // allocating memory for the leftCode
  char* leftCode = malloc(strlen(code) + 2);
  strcpy(leftCode, code);
  strcat(leftCode, "0"); // append 0 to the left code

  // call the buildDic on the left child with the accumulated code
  buildDict(root->left, leftCode, dict);

  // allocating memory for the rightCode
  char* rightCode = malloc(strlen(code) + 2);
  strcpy(rightCode, code);
  strcat(rightCode, "1"); // append 1 to the right code

  // call the buildDic on the left child with the accumulated code
  buildDict(root->right, rightCode, dict);
  return dict;
}

char* huffman_encode_tests(size_t len, const char data[len]){

  char* characters = malloc(len + 1);
  int frequency[256] = {0};

  // printf("length = %li\n",strlen(characters));
  if (characters == NULL){
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  // read the characters and copy them to the characters array
  for (size_t i = 0; i < len; i++){
    characters[i] = *(&data[i]);
  }

  // Count the frequency of each character, using their ASCII value as an index in the frequencies array.
  // All the characters that are not in the characters array will have 0 as their frequency (from the initialization)
  for (size_t i = 0; i < len; i++){
    frequency[(unsigned char)characters[i]]++;
  }
  int array_size = 0;
  //get the number of unique characters
  for (size_t i = 0; i < 256; i++){
    if (frequency[i] > 0){
      array_size++;
    }
  }

  //the frequencies array is initialized with zeros
  int frequencies[array_size];
  memset(frequencies, 0, array_size * sizeof(int));
  //filter the frequency array to only get the characters that have frequency > 0
  size_t j = 0;
  for (size_t i = 0; i < 256; i++){
    if (frequency[i] > 0){
      frequencies[j] = frequency[i];
      characters[j] = (char)i;
      j++;
    }
  }
  //pass the character array, the frequency array and the size of the array of the heap to create the Huffman Tree (root is a global variable)
  root = buildHuffmanTree(characters, frequencies, array_size);

  //allocate space for the Dictionary, initialize it with 0s and call buildDict to create it
  char** dict = malloc(256 * sizeof(char*));
  memset(dict, 0, 256 * sizeof(char*));
  dict = buildDict(root, "", dict);

  buildDict(root, "", dict);
////////////////////////////////All the print statements in a readable form//////////////////////////////////////////////////////////////////////////////////

char* coded = malloc(sizeof(char)* len * 2000);
int coded_index = 0;

  //iterate of the given string and encode it using the dictionary
  for (size_t i = 0; i < len; i++){
    for (size_t j = 0; j < 256; j++){
      //check if the pointer of the code at the index j (aka the ASCII number of a character) not 0 and if it's the same character as the one
      //in the given data string, if it is then concatenate it to the result (decoded)
      if (dict[j] != 0 && data[i] == (char)j){
        //int dict_len = strlen(dict[j]);
        //strncat(coded, dict[j],dict_len);
        strcat(coded, dict[j]);
        coded_index += strlen(dict[j]);
        break;
      }
    }
  }

return coded;
}


void huffman_decode_tests(size_t len, const char data[len]) {
    char* result = malloc(sizeof(char)* len * 2000);
    struct MinHNode* curr = root;
    int index = 0;

    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '0'){
            curr = curr->left;
        }
        else{
            curr = curr->right;
        }
        if (curr->left == NULL && curr->right == NULL) {
            result[index] = curr->character;
            index++;
            curr = root;
        }
    }
    result[index] = '\0';

    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);

    double time = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );
    printf("Excution Time of Decode: %.9f\n",time);
    printf("Decoded String: ");
    for(size_t i = 0; i< 2000; i++) {
           printf("%c", result[i]);
	}
    printf("\n");

}



/////////////////////////////////////////Encode and Decode handling the file inputs//////////////////////////////////////////////////////////////////////////
void huffman_encode(size_t len, const char data[len], FILE* outputFile, int to_print){

  struct timespec start;
  clock_gettime (CLOCK_MONOTONIC,&start);


  char* characters = malloc(len + 1);
  int frequency[256] = {0};

  // printf("length = %li\n",strlen(characters));
  if (characters == NULL){
    fprintf(stderr, "Memory allocation failed\n");
    return;
  }

  // read the characters and copy them to the characters array
  for (size_t i = 0; i < len; i++){
    characters[i] = *(&data[i]);
  }

  // Count the frequency of each character, using their ASCII value as an index in the frequencies array.
  // All the characters that are not in the characters array will have 0 as their frequency (from the initialization)
  for (size_t i = 0; i < len; i++){
    frequency[(unsigned char)characters[i]]++;
  }
  int array_size = 0;
  //get the number of unique characters
  for (size_t i = 0; i < 256; i++){
    if (frequency[i] > 0){
      array_size++;
    }
  }

  //the frequencies array is initialized with zeros
  int frequencies[array_size];
  memset(frequencies, 0, array_size * sizeof(int));
  //filter the frequency array to only get the characters that have frequency > 0
  size_t j = 0;
  for (size_t i = 0; i < 256; i++){
    if (frequency[i] > 0){
      frequencies[j] = frequency[i];
      characters[j] = (char)i;
      j++;
    }
  }
  //pass the character array, the frequency array and the size of the array of the heap to create the Huffman Tree (root is a global variable)
  root = buildHuffmanTree(characters, frequencies, array_size);

  //allocate space for the Dictionary, initialize it with 0s and call buildDict to create it
  char** dict = malloc(256 * sizeof(char*));
  memset(dict, 0, 256 * sizeof(char*));
  dict = buildDict(root, "", dict);

  buildDict(root, "", dict);

////////////////////////////////////////////////Writing in the output files//////////////////////////////////////////////////////////////////////////////////
 if(to_print == 1){
  fprintf(outputFile, "%ld\n", sizeof(frequencies)/4);
  for (size_t i = 0; i < 256; i++){
    if (frequency[i] > 0){
      fprintf(outputFile, "%d ", frequency[i]);
    }
  }
  fprintf(outputFile, "\n");

  for(int i = 0; i < array_size; i++){
      fprintf(outputFile, "%c ", characters[i]);
  }
  fprintf(outputFile, "\n\n");
  for (size_t i = 0; i < len; i++){
      for (size_t j = 0; j < 256; j++){
          if (dict[j] != 0 && data[i] == (char)j){
              fprintf(outputFile, "%s", dict[j]);
              break;
          }
      }
  }
    fprintf(outputFile, "\n = \n%s\n\n", data);

  fprintf(outputFile, "Print the created dictionary: \n");
  fprintf(outputFile, "Symbol | Kodierung\n");
  fprintf(outputFile, "-------+----------\n");

  for (size_t j = 0; j < 256; j++){
    if (dict[j] != 0){
      fprintf(outputFile, "  %c    | %s\n", (char)j, dict[j]);
    }
  }
}



  struct timespec end;
  clock_gettime (CLOCK_MONOTONIC,&end);

  timee += end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );
}


void huffman_decode(size_t len, const char data[len], FILE* outputFile, int to_print) {
    char* result = (char*)malloc(len + 1);
    struct MinHNode* curr = root;
    int index = 0;

    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    for (int i = 0; data[i] != '\0'; i++) {
        if (data[i] == '0'){
            curr = curr->left;
        }
        else{
            curr = curr->right;
        }
        if (curr->left == NULL && curr->right == NULL) {
            result[index] = curr->character;
            index++;
            curr = root;
        }
    }
    result[index] = '\0';

    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);

    timee += end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );
    if (to_print == 1) {
        fprintf(outputFile, "Decoded String: %s\n", result);
    }
}


//////////////////////////////////////////////////////////////////////Version 2 of Encode which uses another version of buildDict/////////////////////

void buildDict2(const struct MinHNode* node, size_t len, struct Pair dict[len]){
    const struct MinHNode* stack[len + 1];
    char* binStack[len + 1];
    int stIndex = 0;
    int dictIndex = 0;
    char* binCode = "";
    // inorder-iterative variant of traversing a binary tree
    while(stIndex != 0 || node != NULL){
        if(node != NULL){
            stack[stIndex] = node;
            char* temp = binCode;
            binStack[stIndex] = temp;
            stIndex++;
            node = node->left;
            // Because the function is traversing in this if-statement always to the left child of a node
            // a "0" gets appended to the current String
            binCode = malloc(strlen(binCode) + 1);
            strcpy(binCode, temp);
            strcat(binCode, "0");
        } else {
            stIndex--;
            node = stack[stIndex];
            binCode = binStack[stIndex];

            // the if-statement translates to if a node is a leave, the first character and its current binCode
            // is saved in the dictionary
            if(node->left == NULL){
                dict[dictIndex].character = node->character;
                dict[dictIndex].binCode = malloc(strlen(binCode));
                strcpy(dict[dictIndex].binCode, binCode);
                dictIndex++;
            }
            node = node->right;
            char* temp = binCode;
            // at the end the function always goes to the right child and so "1" is appended to the current String
            // it could be that the rightChild is Null and we did too much with the binary code,
            // but it gets handled with the general if-statement and the values of the next-inorder node gets popped from stack
            binCode = malloc(strlen(temp) + 1);
            strcpy(binCode, temp);
            strcat(binCode, "1");
        }
    }
}

// this function gives back the number of distinct elements, there is nothing to look out for here
size_t number_distinct_elements(size_t len, const char data[len]){
    if(len == 0 || len == 1){
        return len;
    }

    size_t i, j, count = 1;
    for(i = 1; i < len; i++){
        for(j = 0; j < i; j++){
            if(data[i] == data[j]){
                break;
            }
        }
        if(i == j){
            count++;
        }
    }
    return count;
}

// This function copies all distinct elements into distincted in order of data
void distinct_array(size_t newLen, size_t len, char distincted[newLen], const char data[len]){
    size_t i, j, z = 1;
    distincted[0] = data[0];
    for(i = 1; i < len; i++){
        for(j = 0; j < i; j++){
            if(data[i] == data[j]){
                break;
            }
        }
        if(i == j){
            distincted[z] = data[i];
            z++;
        }
    }
}

// This function counts the occurences of each distinct elements and stores it in count with the same index as in distincted
void count_into_arr(size_t newLen, size_t len, int count[newLen], const char distincted[newLen], const char data[len]){
    for(size_t i = 0; i < len; i++){
        for (size_t j = 0; j < newLen; ++j) {
            if(data[i] == distincted[j]){
                count[j] = count[j] + 1;
                break;
            }
        }
    }
}

void huffman_encode_2(size_t len, const char data[len], FILE* outputFile, int to_print){

    struct timespec start;
    clock_gettime (CLOCK_MONOTONIC,&start);

    newLen = number_distinct_elements(len, data);
    char distincted[newLen];
    int count[newLen];
    for(size_t i = 0; i < newLen; i++){
        count[i] = 0;
    }
    distinct_array(newLen, len, distincted, data);
    count_into_arr(newLen, len, count, distincted, data);

    // the huffman-tree is generated the same as in the main-function
    struct MinHNode *tree = buildHuffmanTree(distincted, count, (int) newLen);

    // call of the new Dictionary function
    buildDict2(tree, newLen, dictionary);

    // this for-loop counts the new length of the encoded String based on the frequencies of a character
    // and the length of it's new binary-code
    unsigned long lenOfCode = 0;
    for(size_t i = 0; i < newLen; i++){
        char* String = dictionary[i].binCode;
        int j = 0;
        while(dictionary[i].character != distincted[j]){
            j++;
        }
        unsigned int lenOfString = strlen(String);
        lenOfCode = lenOfCode + count[j] * lenOfString;
    }

    // this for-loop copies each character's binary-code into a new String in the same order as in the data-String
    char* encoded = calloc(lenOfCode, sizeof(char));
    int l = 0;
    for(size_t i = 0; i < len; i++){
        char current = data[i];
        size_t j = 0;
        for(; j < newLen; j++){
            if(dictionary[j].character == current){
                break;
            }
        }

        int k = 0;
        while(dictionary[j].binCode[k] != 0){
            encoded[l] = dictionary[j].binCode[k];
            l++;
            k++;
        }
    }
     /////////////////////////////////////////////Writing in the output files/////////////////////////////////////////////////////////////////////////////////////
    if(to_print == 1){
        fprintf(outputFile, "%ld\n", newLen);
        for (size_t i = 0; i < newLen; i++){
            if (count[i] > 0){
                fprintf(outputFile, "%d ", count[i]);
            }
        }
        fprintf(outputFile, "\n");

        for(size_t i = 0; i < newLen; i++){
            fprintf(outputFile, "%c ", distincted[i]);
        }
        fprintf(outputFile, "\n\n");
        fprintf(outputFile,"%s", encoded);
        fprintf(outputFile, "\n = \n%s\n\n", data);
    }




    struct timespec end;
    clock_gettime (CLOCK_MONOTONIC,&end);

    timee += end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );

}
void huffman_encode_2_tests(size_t len, const char data[len]){

    newLen = number_distinct_elements(len, data);
    char distincted[newLen];
    int count[newLen];
    for(size_t i = 0; i < newLen; i++){
        count[i] = 0;
    }
    distinct_array(newLen, len, distincted, data);
    count_into_arr(newLen, len, count, distincted, data);

    // the huffman-tree is generated the same as in the main-function
    struct MinHNode *tree = buildHuffmanTree(distincted, count, (int) newLen);

    // call of the new Dictionary function
    buildDict2(tree, newLen, dictionary);

    // this for-loop counts the new length of the encoded String based on the frequencies of a character
    // and the length of it's new binary-code
    unsigned long lenOfCode = 0;
    for(size_t i = 0; i < newLen; i++){
        char* String = dictionary[i].binCode;
        int j = 0;
        while(dictionary[i].character != distincted[j]){
            j++;
        }
        unsigned int lenOfString = strlen(String);
        lenOfCode = lenOfCode + count[j] * lenOfString;
    }

    // this for-loop copies each character's binary-code into a new String in the same order as in the data-String
    char* encoded = calloc(lenOfCode, sizeof(char));
    int l = 0;
    for(size_t i = 0; i < len; i++){
        char current = data[i];
        size_t j = 0;
        for(; j < newLen; j++){
            if(dictionary[j].character == current){
                break;
            }
        }

        int k = 0;
        while(dictionary[j].binCode[k] != 0){
            encoded[l] = dictionary[j].binCode[k];
            l++;
            k++;
        }
    }

}


