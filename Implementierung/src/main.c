#include "huffman.h"

#include <stddef.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <sys/stat.h>


//The Tree is a global variable so that it can be accessed from encode and decode
struct MinHNode* root;

double timee; 

int main(int argc, char* argv[]){

    //declare helper variables
    int c;
    int index = -1;
    opterr = 0;
    char* string = NULL;
    char* outputFileString = NULL;
    FILE* inputFile = NULL;
    FILE* outputFile;
    int dflag = 0;
    int Bflag = 0;
    int oflag = 0;
    int hflag = 0;
    int B_counter = 1;
    int to_print = 0;
    int V_counter = 0;

    //necessary for --help
    static const struct option long_options[] =
            {
                    { "help", no_argument,       0, 0 },
                    { 0, 0, 0, 0 }
            };

    //parse command line arguments
    while ((c = getopt_long (argc, argv, "dV:B::o:h", long_options, &index)) != -1){
        switch (c)
        {
            case 'd':
                dflag = 1;
                break;
            case 'V':
                 if (optarg == NULL){
                    fprintf(stderr, "\nUsage:                   %s [-d] [-V <version>] [-B [cycles]] [-h] [--help] input_file -o <output_file> \n\n"
                                    "For more information :   '%s -h' or '%s --help'\n\n", argv[0], argv[0], argv[0]);
                    return EXIT_FAILURE;
                } else {
                    V_counter = atoi(optarg);
                    if (V_counter != 0 && V_counter != 1){
                        printf("There are only two implementations of Encode, V0 and V1. Choose one of them.\n");
                        return EXIT_FAILURE;
                    }
                }
                break;
            case 'B':
                if (optarg == NULL){
                    B_counter = 1;
                } else {
                    B_counter = atoi(optarg);
                }
                Bflag = 1;
                break;
            case 'o':
                outputFileString = optarg;
                oflag = 1;
                break;
            case 'h':
                hflag = 1;
                goto check_h_flag;
                break;
            case 0 :
                hflag = 1;
                printf("helppp\n");
                goto check_h_flag;
                break;
            default:
                fprintf(stderr, "\nUsage:                   %s [-d] [-V <version>] [-B [cycles]] [-h] [--help] input_file -o <output_file> \n\n"
                                              "For more information :   '%s -h' or '%s --help'\n\n", argv[0], argv[0], argv[0]);
                return EXIT_FAILURE;
        }
    }

    //parse positional arguments, i.e. open input file
    for (int i = optind; i < argc; ++i) {
       // printf("non-option: %s\n", argv[i]);
            if (!(inputFile = fopen(argv[i], "r"))){
                perror("Error opening file");
                return 1;
            } else {
                struct stat statbuf ;
                if (fstat (fileno (inputFile) , &statbuf)) {
                    fprintf (stderr, "Error retrieving file stats \n");
                    goto cleanup;
                }
                if (!S_ISREG (statbuf.st_mode) || statbuf.st_size <= 0) {
                    fprintf (stderr, "Error processing file%s : Not a regular file or invalid size \n", argv[i]);
                    goto cleanup;
                }
                if (!(string = malloc (statbuf.st_size + 1))) {
                    fprintf (stderr, "Error reading file : Could not allocate enogh memory\n");
                    goto cleanup;
                }
                if (fread (string, 1, statbuf.st_size, inputFile) != (size_t) statbuf.st_size) {
                    fprintf (stderr , "Error reading file");
                    free (string);
                    string = NULL;
                    goto cleanup;
                }
                string[statbuf.st_size] = '\0';
                continue;
            }
    }

    //input file argument necessary, prints error message if missing
    if (optind >= argc) {
        fprintf(stderr, "\nUsage:                    %s [-d] [-V <version>] [-B [cycles]] [-h] [--help] input_file -o <output_file> \n\n"
                                        "For more information :   '%s -h' or '%s --help'\n\n", argv[0], argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    //change output file name
    if(oflag == 0){
        fprintf(stderr, "\nUsage:                    %s [-d] [-V <version>] [-B [cycles]] [-h] [--help] input_file -o <output_file> \n\n"
                                        "For more information :   '%s -h' or '%s --help'\n\n", argv[0], argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    if (!(outputFile = fopen (outputFileString, "w"))) {
        perror ("Error opening file");
        return EXIT_FAILURE;
    }

    //to be extended, should print help message to terminal
    check_h_flag:
    if (hflag == 1){
        printf("\nUsage:         %s [options] input_file -o <output_file>\n\n"
                      "Description:   Depending on whether -d is set or not ./Huffman en- or decodes a given input_file using the\n"
                      "               Huffman coding method and writes the result in the ouput_file\n\n"
                      "Options:       -d               if set: decode content of input_file, if not: encode it\n"
                      "               -V <version>     define which version should be used\n"
                      "               -B [cycles]      print out run-time of function,\n"
                      "                                if [cycles] is given, it defines how often function is called\n"
                      "               -h               print out this help message\n"
                      "               --help           print out this help message\n\n"
                      "Example:       content of ./test/string.txt: ABRAKADABRAB \n\n"
                      "               ./Huffman -B ./test/string.txt ./encoded_string\n\n"
                      "               content of ./encoded_string:\n\n"
                      "               5                                              \\\n"
                      "               5 3 1 1 2                                      |   information for decoding\n"
                      "               A B D K R                                      /\n\n"
                      "               0101100111001111010110010                      \\\n"
                      "               =                                              |   encoded and decoded string\n"
                      "               ABRAKADABRAB                                   /\n\n\n"
                      "               Print the created dictionary:                  \\\n"
                      "               Symbol | Kodierung                             |\n"
                      "               -------+----------                             |\n"
                      "                 A    | 0                                     |  dictionary table\n"
                      "                 B    | 10                                    |\n"
                      "                 D    | 1111                                  |\n"
                      "                 K    | 1110                                  |\n"
                      "                 R    | 110                                   /\n\n\n"
                      ,argv[0]);
        return 0;
    }

   

    //encode, as -d isn't set
    if (dflag == 0){
        if(B_counter == 1){
            to_print = 1;
        }
         if (V_counter == 1) {
            huffman_encode(strlen(string) - 1, string, outputFile, to_print);
        } else {
            huffman_encode_2(strlen(string) - 1, string, outputFile, to_print);
        }
        if (Bflag == 1){
            for(int p = 1; p < B_counter; p++){
                if(p == B_counter - 1){
                    to_print = 1;
                }
                if (V_counter == 1) {
                    huffman_encode(strlen(string) - 1, string, outputFile, to_print);
                } else {
                    huffman_encode_2(strlen(string) - 1, string, outputFile, to_print);
                }
            }
            printf("Number of huffman_encode calls: %d\n", B_counter);
            printf("Execution Time of Encode: %.9f\n", timee);
        }
    }

    //decode, as -d is set
    if (dflag == 1) {
        long freqSizee = 0;
        int newLineCounter = 0;
        int k = 0;
        int i = 0;
        int j = 0;
        char toFreq[8];
        long freq = 0;
        char* endptr;
        int indexFreq = 0;
        double timeee = 0;

        //first line: where length of frequencies[] and characters[] is written
        for (; string[k] != '\n';) {
            k++;
        }
        char freqSize[k];
        for (; i < 1000; i++) {
            if (string[i] == '\n') {
                freqSize[i] = '\0';
                freqSizee = strtol(freqSize, &endptr, 10);
                newLineCounter++;
                break;
            } else{
                freqSize[i] = string[i];
            }
        }

        //create frequencies[] and characters[] to build HuffmanTree
        int frequenciesDecode[freqSizee];
        memset(frequenciesDecode, 0, freqSizee * sizeof(int));
        char* charactersDecode = malloc(freqSizee * sizeof(char));
        i++;

            //iterate over remaining input file to access information
            while (true){

                //second line: fill frequencies[]
                if (newLineCounter == 1) {
                    if (string[i] == '\n') {
                        i++;
                        newLineCounter++;
                        continue;
                    } else {
                        toFreq[0] = string[i];
                        i++;
                        if (string[i] == ' ') {
                            toFreq[1] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[1] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[2] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[2] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[3] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[3] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[4] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[4] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[5] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[5] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[6] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            toFreq[6] = string[i];
                            i++;
                        }
                        if (string[i] == ' ') {
                            toFreq[7] = '\0';
                            freq = strtol(toFreq, &endptr, 10);
                            frequenciesDecode[indexFreq] = freq;
                            indexFreq++;
                            i++;
                            continue;
                        } else {
                            printf("input string is too long to decode it\n");
                            return EXIT_FAILURE;
                        }
                    }
                }

                //third line: fill frequencies[], then print frequencies[] and characters[]
                if (newLineCounter == 2) {
                    for (; j < freqSizee; j++){
                        charactersDecode[j] = string[i + 2 * j];
                    }
                    newLineCounter++;
                    i = i + 2* j;

                    i++; i++;
                    j = i;
                }

                //third line: encoded string, get length k of it
                if (newLineCounter == 3) {
                    for (k = i; string[i] != '\n';) {
                        i++;
                    }
                    k = i - k;
                    break;
                }
            }


            //terminate string
            string[i] = '\0';
          
          struct timespec start;
          clock_gettime (CLOCK_MONOTONIC,&start);

        //to_print necessary for printing only once, though decode gets called more often
            if (B_counter == 1) {
                to_print = 1;
            }

        //core function
            root = buildHuffmanTree(charactersDecode, frequenciesDecode, freqSizee);
            huffman_decode(k, &string[j], outputFile, to_print);

            //print execution time, as -B is set, B_counter describes optional arg of -B -> number of function calls
            if(Bflag == 1) {
                for (int p = 1; p < B_counter; p++){
                    if (p == B_counter - 1){
                        to_print = 1;
                    }
                    huffman_decode(k, &string[j], outputFile, to_print);
                }
                printf("Number of huffman_decode calls: %d\n", B_counter);
                
            }
      struct timespec end;
      clock_gettime (CLOCK_MONOTONIC,&end);

      timeee = end . tv_sec - start . tv_sec + 1e-9 * (end . tv_nsec - start . tv_nsec );
      if(Bflag == 1){
      printf("Excution Time of Decode: %.9f\n", timeee);
      }
    }

    cleanup:
    if (inputFile){
        fclose (inputFile);
    }

  
    return 0;
}

