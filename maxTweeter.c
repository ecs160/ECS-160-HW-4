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

// gets specific item in given row, given the column index
const char* getfield(char* row, int col_index) {
  const char* tok;
  for (tok = strtok(row, ",");
       tok && *tok;
       tok = strtok(NULL, ",\n")) {
    if (!--col_index)
      return tok;
  }
  return NULL;
}

// returns index of "name" in line
const int getNameIndex(char* line) {
  int nameIndex = -1;     // use -1 to denote that name has not been found yet
  int curIndex = 0;       // keep track of which line field we are at
  
  char* token = strtok(line, ",");
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
    token = strtok(NULL, ","); // NULL as first arg means keep searching cur string
  }
  return nameIndex;
}

int main(int argc, const char *argv[]) {
  char *fileName;         // name of our file
  char line[MAX_CHAR];   // line with max_char size
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
    strcpy(fileName, argv[1]);
    // if we have proper command line args, check it's csv and exists
    if(!fileExists(fileName) || !isCSV(fileName)){
      return errorMsg();
    }
  }
  
  // printf("File Name: %s\n", fileName);
  
  // Part 2 - Check Valid lines
  // check lines and make sure valid
  // only one "name" column
  // tolower each section???
  // check line size does not exceed max_char
  
  FILE* file = fopen(fileName, "r");
  
  if(!file) {
    return errorMsg();
  }
  
  // check that file has content in it
  if(!fgets(line, MAX_CHAR, file)){
    printf("!!!!!!WE ARE IN EMPTY FILE!!!!\n");
    return errorMsg();
  }
  
  // on successful fgets, line terminated with newline
  // if no newline, means exceeded buffer size
  if(line[strlen(line) - 1] != '\n') {
    printf("Overflow detection!!!!!!");
    return errorMsg();
  }
  
  // printf("%s\n", line);
  
  // char test[] = "lulu,sean,poo,prem";
  // char testFAIL[] = "name, name";  // space messes things up
  
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
  // create that shit
  // another loop to insert the names in hashtable
  
  free(fileName);
  
  return 0;
}
