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

// checks that file exists
const bool fileExists(const char *fileName) {
  return(access(fileName, F_OK ) == 0);
}

// returns index of "name" in line
// POSSIBLE ERROR: WITH STRTOK AGAIN IF THERE IS BLANK IN BETWEEN HEADERS
// const int OLDgetNameIndex(char* line) {
//     int nameIndex = -1;     // use -1 to denote that name has not been found yet
//     int curIndex = 0;       // keep track of which line field we are at
//
//     char* token = strtok(line, ",");
//     while(token) {
//         printf("%s\n", token);
//         if(strcmp(token, "name") == 0){
//             if(nameIndex == -1) {   // this means "name" has never been seen
//                 nameIndex = curIndex;
//             } else {
//                 return -1;          // otherwise we saw multiple so return
//             }
//         }
//         curIndex++;
//         token = strtok(NULL, ","); // NULL as first arg means keep searching cur string
//     }
//     return nameIndex;
// }

const int getNameIndex(char* line) {
  int nameIndex = -1;     // use -1 to denote that name has not been found
  int curIndex = 0;       // keep track of which column we are at
  
  char* token = strsep(&line, ",");
  while(token) {
    printf("%s\n", token);
    if(strcmp(token, "name") == 0){
      if(nameIndex == -1) {   // this means "name" has never been seen
        nameIndex = curIndex;
      } else {
        return -1;          // otherwise we saw multiple so return
      }
    }
    curIndex++;
    token = strsep(&line, ","); // NULL as first arg means keep searching cur string
  }
  return nameIndex;
}

// gets specific item in given row, given the column index
// const char* OLDgetfield(char* row, int col_index) {
//     const char* tok;
//     for (tok = strtok(row, ",");
//         tok && *tok;
//         tok = strtok(NULL, ",\n")) {
//       printf("before colindex: %d\n",col_index);
//             if (!--col_index) {
//         printf("after colindex: %d\n",col_index);
//         return tok;
//       }
//
//     }
//     return NULL;
// }

// gets specific item in given row, given the column index
const char* getfield(char* row, int col_index) {
  const char* tok = strsep(&row, ",");
  
  while(col_index != 0 || (tok && *tok)) {
    if (!col_index) {    // found the correct column
      return tok;
    }
    col_index--;
    tok = strsep(&row, ",\n");
  }
  return NULL;
}

// =============================  MAIN ===============================
int main(int argc, const char *argv[]) {
  char *fileName;         // name of our file
  char line[MAX_CHAR];   // line with max_char size
  // char* line = (char*) malloc(sizeof(char) * MAX_CHAR);
  int nameIndex;          // index of "name" line field
  
  // Part 1 - Getting the File
  // get the file from command line
  // make sure file is valid/exists / correct arg numbers given
  // check that file is CSV file
  
  // make sure we have 2 command line arguments
  if (argc != 2) {
    return errorMsg();
  } else {
    fileName = (char*) malloc(sizeof(char) * strlen(argv[1]));
    if(!fileName) {
      return errorMsg();
    }
    strcpy(fileName, argv[1]);
    // if we have proper command line args, check it's csv and exists
    if(!fileExists(fileName) || !isCSV(fileName)){
      return errorMsg();
    }
  }
  
  // printf("File Name: %s\n", fileName);
  
  // Part 2 - Check Valid Lines
  // Open file and make sure it exists
  // check header and make sure valid
  // only one "name" column
  // check line size does not exceed max_char
  
  FILE* file = fopen(fileName, "r");
  
  if(!file) {
    return errorMsg();
  }
  
  // check that file has content in it, otherwise error
  if(!fgets(line, MAX_CHAR, file)){
    printf("!!!!!!WE ARE IN EMPTY FILE!!!!\n");
    return errorMsg();
  }
  
  // on successful fgets, the string is terminated with newline
  // if no newline, means buffer size exceeded
  if(line[strlen(line) - 1] != '\n') {
    printf("Overflow detection!!!!!!");
    return errorMsg();
  }
  
  printf("%s\n", line);
  
  // char test1[] = "lulu,sean,,poo,prem";
  // char test2[] = "bea,,name";  // space messeses indexing so need strsep instead strtok
  // char test3[] = "name,,name";
  // char test4[] = "lulu,sean,poo,prem";
  
  nameIndex = getNameIndex(line);
  if(nameIndex == -1) {
    return errorMsg();
  }
  
  printf("%d\n", nameIndex);
  
  while(fgets(line, MAX_CHAR, file)) {
    printf("Name: %s\n", getfield(line, nameIndex));
  }
  
  // Part 3 - Get Unique Names
  // loop through entire file to collect number of names
  
  // Part 4 - Create Hashtable
  // store names in hashtable
  // another loop to insert the names in hashtable
  
  free(fileName);
  
  return 0;
}
