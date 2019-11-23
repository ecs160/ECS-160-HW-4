#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define ERROR_MESSAGE "Invalid Input Format\n"
#define MAX_CHAR 1024

int errorMsg() {
    printf("%s", ERROR_MESSAGE);
    return 1;
}

// checks that file extension is csv
const bool isCSV(const char *fileName) {
    const char *dot = strrchr(fileName, '.');
    if(!dot || dot == fileName) return "";

    // POSSIBLE TODO: case sensitive checking of extension name
    return strcmp(dot + 1, "csv") == 0;
}

const bool fileExists(const char *fileName) {
    // returns true if file exists
    // printf("!!!!!!! IM IN FILE EXISTS!!!!\n");
    // printf("%d, \n", access(fileName, F_OK ));
    return(access(fileName, F_OK ) == 0);
}

const char* getfield(char* line, int num) {
    const char* tok;
    for (tok = strtok(line, ",");
        tok && *tok;
        tok = strtok(NULL, ",\n"))
    {
    if (!--num)
        return tok;
    }
    return NULL;
}

const int getNameIndex(char* header) {
    int nameIndex = -1;
    int curIndex = 0;
    char* token = strtok(header, ",");

    while(token) {
        printf("%s\n", token);
        if(strcmp(token, "name") == 0){
            // this means name has never been seen
            if(nameIndex == -1) {
                nameIndex = curIndex;
            } else {
                return -1;
            }
        }
        curIndex++;
        token = strtok(NULL, ","); // NULL as first arg means keep searching cur string
    }
    return nameIndex;
}

int main(int argc, const char *argv[]) {
    char *fileName;
    char header[1024];
    int nameIndex;
    int charCount;
    // Part 1 - Getting the File
    // get the file from command line
    // make sure file is valid/exists / correct arg numbers given
    // check that file is CSV file
    // make sure we have 2 command line arguments
    if (argc != 2) {
        return errorMsg();
    } else {
        fileName = (char*) malloc(sizeof(char) * strlen(argv[1]));
        strcpy(fileName, argv[1]);
        // if we have proper command line args, check it's csv and exists
        if(!fileExists(fileName) || !isCSV(fileName)){
            return errorMsg();
        }
    }

    // printf("File Name: %s\n", fileName);

    // Part 2 - Check Valid Headers
    // check headers and make sure valid
    // only one "name" column
    // tolower each section

    FILE* file = fopen(fileName, "r");

    if(!file) {
        return errorMsg();
    }

    printf("Pass Valid File\n");

    char c;
    for (c = getc(file); c != EOF; c = getc(file)) {
        charCount++;
    }

    if(charCount > MAX_CHAR) {
        printf("CHARCOUNT TOO MUCH: %d\n", charCount);
        return errorMsg();
    }

    if(!fgets(header, MAX_CHAR, file)){
        printf("!!!!!!WE ARE IN EMPTY FILE!!!!\n");
        return errorMsg();
    }


    // if(strcmp(header[strlen(header) - 1], '\n') == 0){
    //   printf("valid\n");
    // }else{
    //   printf("invalid\n");
    // }


    printf("%s\n", header);
    // char test[] = "lulu,sean,poo,prem";
    // // char testFAIL[] = "name, name";  // space messes things up
    //
    // nameIndex = getNameIndex(test);
    // if(nameIndex == -1) {
    //   return errorMsg();
    // }
    // printf("%d\n", nameIndex);





  // Part 3 - Get Unique Names
  // loop through entire file to collect number of names

  // Part 4 - Create Hashtable
  // create that shit
  // another loop to insert the names in hashtable

  free(fileName);

  return 0;
}
