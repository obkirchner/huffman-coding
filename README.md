# Description

### Usage:           `./main [options] <input_file> -o <output_file>`
             

Depending on whether `-d` is set or not `./main` encodes or decodes a given input_file using the
Huffman coding method and writes the result in the ouput_file.


|Options                         |Description                                         |
|-------------------------------|-----------------------------                        |
|`-o`                           |specify the output_file                              |
|`-d`                           |if set: decode content of input_file, if not: encode |
|`-V<version>`                  |define which version should be used                  |
|`-B[cycles]`                   |print out run-time of function, if `[cycles]` is given, it defines how often function is called                                                                                |
|`-h`                           |print out this description as a help message         |
|`--help`                       |print out this description as a help message         |

## Example:       
 Content of the test file `test.txt` :  `./src/input_output/test.txt: ABRAKADABRAB`

   

 1. Encode: `./main ./src/input_output/test.txt -o ./src/input_output/encoded_string.txt`
 2. Content of `./src/input_output/encoded_string.txt`:

                           5                                              \
                           5 3 1 1 2                                      |   information for decoding
                           A B D K R                                      /

                           0101100111001111010110010                      \
                           =                                              |   encoded and decoded string
                           ABRAKADABRAB                                   /



                           Print the created dictionary:                  \
                           Symbol | Kodierung                             |
                           -------+----------                             |
                             A    | 0                                     |  dictionary table
                             B    | 10                                    |
                             D    | 1111                                  |
                             K    | 1110                                  |
                             R    | 110                                   /

3. Decode: `./main -d ./src/input_output/encoded_string.txt -o ./src/input_output/decoded_string.txt`
4. Content of `./src/input_output/decoded_string.txt`:
                           Decoded String: ABRAKADABRAB
