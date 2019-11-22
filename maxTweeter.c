#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define ERROR_MESSAGE "Invalid Input Format\n"

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

int main(int argc, const char *argv[]) {
	char *fileName;
	// Part 1 - Getting the File
	// get the file from command line
	// make sure file is valid/exists / correct arg numbers given
	// check that file is CSV file

	// make sure we have 2 command line arguments
	if (argc != 2) {
		printf("%s", ERROR_MESSAGE);
		return 1;
	} else {
		fileName = (char*) malloc(sizeof(char) * strlen(argv[1]));
		strcpy(fileName, argv[1]);

		// if we have proper command line args, check it's csv and exists
		if(!fileExists(fileName) || !isCSV(fileName)){
			printf("%s", ERROR_MESSAGE);
			return 1;
		}
	}

	printf("File Name: %s\n", fileName);

//	FILE* stream = fopen("")

	// Part 2 - Check Valid Headers
	// check headers and make sure valid
	// only one "name" column
	// tolower each section

	// Part 3 - Get Unique Names
	// loop through entire file to collect number of names

	// Part 4 - Create Hashtable
	// create that shit
	// another loop to insert the names in hashtable

	free(fileName);

	return 0;
}
