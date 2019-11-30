#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define ERROR_MESSAGE "Invalid Input Format\n"
#define MAX_CHAR 1024

int errorMsg() {
    printf("%s", ERROR_MESSAGE);
    return -1;
}

// trims newline from fgets
void trimLine(char* line) {
    if(line == NULL || strlen(line) == 0) {
        return;
    }
    printf("cur line in trim[%s]\n", line);
    while(line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\r') {
        line[strlen(line) - 1] = '\0';
    }
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

// finds position of an input string and returns the index in the array
int findStringPos(char *headers[], int colCount, char *input) {
    for(int i = 0; i < colCount; ++i) {
        if(strcmp(headers[i], input) == 0) {
            return i;
        }
    }
    return -1;
}

// adds an input to end of the array
int addHeader(char *headers[], int *colCount, char *input) {
    headers[*colCount] = (char*) malloc(sizeof(char) * (strlen(input) + 1));
    *colCount += 1;

    return *colCount;
}

// get index of "name" column
const int getNameIndex(char* line) {
    int nameIndex = -1;     // use -1 to denote that name has not been found
    char *headers[MAX_CHAR];
    int colCount = 0;

  // delimiters also need \r or \n in case name is at the last column
    char* token = strsep(&line, ",");
    while(token) {
        printf("colcount:%d\n", colCount);
        int pos = findStringPos(headers, colCount, token);
        if(pos == -1) {
            addHeader(headers, &colCount, token);
        } else {
            printf("findStringDup token:[%s]\n", token);
            return -1;
        }
         // if we're here means there were no duplicates of any found
        if(strcmp(token, "name") == 0){
            nameIndex = colCount - 1;
        }
        token = strsep(&line, ",");
    }
    printf("name ind: %d\n", nameIndex);
    return nameIndex;
}


// gets specific item in given row, given the column index
const char* getField(char* row, int col_index) {
    const char* token = strsep(&row, ",");

    while(col_index != 0 || (token && *token)) {
        if (!col_index) {    // we found the correct column if index == 0
            // printf("reading in %s\n", token);
            return token;
        }
        // keep going/decrement since we aren't in correct column
        col_index--;
        token = strsep(&row, ",");  // get next token
    }
    return NULL;
}

// int isValidQuotedHeader(char* item) {
//     // return 0 if invalid header quotes
//     // return 1 if no quotes in header ex: name
//     // return 2 if quotes "name"
//     char* quoteIndexArr = (char*) malloc(sizeof(char) * strlen(item));
//     char* pch = strchr(item, '"');
//     int i = 0;
//     bool atEnds;    // ex: "item"
//     bool inMiddle;  // ex: "ite"m" INVALID
//     bool
//
//     while(pch) {
//         quoteIndexArr[i] = pch - item + 1;
//         pch=strchr(pch + 1,'"');
//         ++i;
//     }
//
//
//
//     free(quoteIndex);
//     return 0;
// }


// =================== LINKED LIST =======================
typedef struct node{
    char* name;
    int occurrences;
    struct node* next;
} node_t;

void print_list(node_t * head) {
    node_t * current = head;

    while (current != NULL) {
        printf("%s has an occurrence of: %d\n", current->name, current->occurrences);
        current = current->next;
    }
}

void push(node_t * head, const char* name) {
    node_t * current = head;
    while (current != NULL) {
        //if name exists already
        // printf("cur name: %s | compared to name: %s\n", current->name, name);
        if(strcmp(current->name, name) == 0){
            //increment the number of occurrences
            // printf("match: %d!\n", current->occurrences + 1);
            current->occurrences = current->occurrences + 1;
            return;
        }
        if(current->next == NULL)
            break;
        current = current->next;
    }

    /* now we can add a new variable */
    // printf("creating new node: %s\n", name);
    current->next = malloc(sizeof(node_t));
    if(!current->next){
        printf("%s", ERROR_MESSAGE);
        return;
    }
    current->next->name = malloc(sizeof(char) * (strlen(name) + 1));
    if(!current->next->name) {
        printf("%s", ERROR_MESSAGE);
        return;
    }

    strcpy(current->next->name, name);
    current->next->occurrences = 1;
    current->next->next = NULL;
}

void init_head(node_t * head, const char* name){
    // node_t * current = head;
    head->name = malloc(sizeof(char) * (strlen(name) + 1));
    if(!head->name) {
        printf("%s", ERROR_MESSAGE);
        return;
    }

    strcpy(head->name, name);
    head->occurrences = 1;
    head->next = NULL;
}

int pop(node_t ** head) {
    int retval = -1;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->occurrences;
    free(*head);
    *head = next_node;
    return retval;
}

int remove_by_index(node_t ** head, int n) {
    int i = 0;
    int retval = -1;
    node_t * current = *head;
    node_t * temp_node = NULL;

    if (n == 0) {
        return pop(head);
    }

    for (i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->occurrences;
    current->next = temp_node->next;
    free(temp_node);

    return retval;
}

void getLargestOccurrence(node_t ** head){
    node_t * current = *head;
    int cur_i = 0;
    int max = 0;
    int max_i = 0;
    char *max_name;

    while (current != NULL) {
        // printf("%s has an occurrence of: %d\n", current->name, current->occurrences);
        if(current->occurrences > max){
            max = current->occurrences;
            max_i = cur_i;
            max_name = (char*) malloc(sizeof(char) * (strlen(current->name) + 1));

            printf("(current name:%s)\n", current->name);

            if(!max_name) {
                printf("%s", ERROR_MESSAGE);
                return;
            }

            max_name = current->name;
        }
        cur_i++;
        current = current->next;
    }

    printf("big boi named: %s is at index: %d with occurrence of %d\n", max_name, max_i, max);
    printf("removing big boi from linked list.....\n");
    remove_by_index(head, max_i);
    printf("---removal complete\n");
c}

// =============================  MAIN ===============================
int main(int argc, const char *argv[]) {
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
        // if we have proper command line args, check it's csv and exists
        if(!fileExists(argv[1]) || !isCSV(argv[1])) {
            return errorMsg();
        }
    }

    // printf("File Name: %s\n", fileName);

    // Part 2 - Check Valid Lines
    // Open file and make sure it exists
    // check header and make sure valid
    // only one "name" column
    // check line size does not exceed max_char

    FILE* file = fopen(argv[1], "r");

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
        printf("first overflow!!!\n");
        printf("Overflow detection!!!!!!\n");
        return errorMsg();
    }

    // printf("156: %s\n", line);

    // char test1[] = "lulu,sean,,poo,prem";
    // char test2[] = "bea,,name";  // space messeses indexing so need strsep instead strtok
    // char test3[] = "name,,name";
    // char test4[] = "lulu,sean,poo,prem";
    // char test5[] = "";
    // char test6[] = ",";
    // char test7[] = "name,";

    trimLine(line);
    nameIndex = getNameIndex(line);
    if(nameIndex == -1) {
        return errorMsg();
    }

    // printf("%d\n", nameIndex);

    // Part 3 - Get Unique Names
    // loop through entire file to collect number of names
    // while(fgets(line, MAX_CHAR, file)) {
        // printf("Name: %s\n", getField(line, nameIndex));
    // }

    // Part 4 - Create Hashtable
    // store names in hashtable
    // another loop to insert the names in hashtable
    bool init = true;
    node_t * head = NULL;
    head = malloc(sizeof(node_t));
    if (!head)
        return errorMsg();

    line[MAX_CHAR - 1] = '\0';
    while(fgets(line, MAX_CHAR, file)) {
        if(line[MAX_CHAR - 1] != '\0') {
            printf("second overflow!!!!!!!\n");
            printf("Overflow detection!!!!!!\n");
            return errorMsg();
        }
        trimLine(line);
        if(init) {
            // printf("initialiazing the head\n");
            init_head(head, getField(line, nameIndex));
            init = false;
        }else {
            push(head, getField(line, nameIndex));
        }
        // printf("Name: %s\n", getField(line, nameIndex));
    }

    // print_list(head);

    // gets top 10 or if less than 10 as many as possible
    for(int i = 0; i < 10 && head; ++i) {
        getLargestOccurrence(&head);
    }


    return 0;
}
