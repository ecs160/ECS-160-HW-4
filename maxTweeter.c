#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define ERROR_MESSAGE "Invalid Input Format\n"
#define MAX_CHAR 1024
#define MAX_LINES 20000

// =================== LINKED LIST STRUCT =======================
typedef struct node{
    char* name;         // name of tweeter
    int occurrences;    // tweeter count
    struct node* next;  // next node
} node_t;

// =================== HEADER STRUCT =======================
typedef struct header{
    char* headerName;   // name of the header field
    bool isQuoted;      // if header is quoted or not
} header_t;


// ====================== FUNCTIONS ======================
int errorMsg() {    // prints out invalid input error message
    printf("%s", ERROR_MESSAGE);
    return -1;
}

// checks that file extension is csv
const bool isCSV(const char *fileName) {
    const char *dot = strrchr(fileName, '.');
    // dot == fileName in the case that it is curr dir "."
    if(!dot || dot == fileName) {
        return false;
    }
    // POSSIBLE TODO: case sensitive checking of extension name
    return (strcmp(dot + 1, "csv") == 0 || strcmp(dot + 1, "CSV") == 0);
}

// checks that file exists given filepath/name
const bool fileExists(const char *fileName) {
    return(access(fileName, F_OK ) == 0);
}

// trims newline from fgets input and adds null character to the end
void trimLine(char* line) {
    if(line == NULL || strlen(line) == 0) {
        return;
    }
    printf("cur line in trim[%s]\n", line);
    while(line[strlen(line) - 1] == '\n' || line[strlen(line) - 1] == '\r') {
        line[strlen(line) - 1] = '\0';
    }
}

// returns index of 'input' string in the 'headers' array
int findStringPos(header_t headers[], int headerCount, char *input) {
    for(int i = 0; i < headerCount; ++i) {
        if(strcmp(headers[i].headerName, input) == 0) {
            return i;
        }
    }
    return -1;  // -1 means not found
}

// adds an input to end of a array (used for checking duplicates in header)
int addHeader(header_t headers[], int *headerCount, char *input, bool isQuoted) {
    // strlen + 1 to account for null terminator
    headers[*headerCount].headerName = (char*) malloc(sizeof(char) * (strlen(input) + 1));
    headers[*headerCount].isQuoted = isQuoted;
    strcpy(headers[*headerCount].headerName, input);

    *headerCount += 1;

    return *headerCount;
}

// parses a field to get rid of outside quotes if any
char *trimQuotes(char* field, char* buffer, bool* isQuoted) {
    if(field[0] == '"') {
        if(field[strlen(field) - 1] == '"') {
            strncpy(buffer, field + 1, strlen(field) - 2);
            *isQuoted = true;
            return buffer;
        } else {
            return NULL;       // invalid quotes
        }
    }
    strcpy(buffer, field);
    *isQuoted = false;

    return buffer;
}

// count that there are matching quotes in a given string
bool hasMatchingQuotes(const char* string) {
    const char* cur = string;
    int count = 0;

    printf("cur char = %d\n", *cur);

    while(*cur) {
        if(*cur == '"') {
            count++;
        }
        cur++;
    }

    return (count % 2) == 0;
}

// validate header (checks no duplicates items/valid quotes) and get index of "name"
const int valAndGetNameIndex(char* line, header_t headers[], int *headerCount) {
    int nameIndex = -1;     // use -1 to denote that name has not been found
    char buffer[MAX_CHAR];
    bool isQuoted = false;

    char* token = strsep(&line, ",");
    while(token) {
        printf("TOKEN:[%s]\n", token);
         // parse a string to remove outer quotes if any
        char *noQuotes = trimQuotes(token, buffer, &isQuoted);
        printf("trim quote:%s\n", buffer);
        if(noQuotes == NULL) {
            printf("invalid quote parse\n");
            return -1;   // null if invalid quote parse
        }
        printf("after no QUOTES\n");
        // also check that after removing outer quotes, other quotes inside valid
        if(!hasMatchingQuotes(buffer)) {
            printf("quote mismatch!\n");
            return -1;
        }
        printf("afeter has matching quotes\n");
        // find position of token in headers array
        int pos = findStringPos(headers, *headerCount, buffer);
        printf("find string pos: %d\n", pos);
        // create array of headers
        if(pos == -1) {     // did not find the token so add it to headers[headerCount]
            addHeader(headers, headerCount, buffer, isQuoted);
            printf("added header\n");
        } else {
            printf("found duplicate token:[%s]\n", buffer);    // otherwise we found a duplicate item
            return -1;
        }
         // if we're here means there were no duplicates of any found and name just showed up
        if(strcmp(buffer, "name") == 0){
            nameIndex = *headerCount - 1;   // -1 since 0 based
        }
        token = strsep(&line, ","); // get next token
    }

    printf("name ind: %d\n", nameIndex);
    return nameIndex;
}

// gets specific item in given row, given the column index
const char* getField(char* row, int col_index, int *colCount) {
    const char* nameItem = NULL;
    *colCount = 0;

    const char* token = strsep(&row, ",");
    while(token) {
        if (!col_index) {    // we found the correct column if index == 0
            // printf("reading in %s\n", token);
            nameItem = token;
        }
        // keep going/decrement since we aren't in correct column
        col_index--;
        token = strsep(&row, ",");  // get next token
        *colCount += 1;
    }
    return nameItem;
}

// ensure name Item is either quoted or not quoted based on name header
bool nameItemQuoteCorrect (const char* nameItem, bool isQuoted, char* buffer) {
    printf("in nameItemQuoteCorrect\n");
    if(isQuoted) {
        if(nameItem[0] != '"' || nameItem[strlen(nameItem) - 1] != '"') {
            return false;
        }
        strncpy(buffer, nameItem + 1, strlen(nameItem) - 2);    // strip quotes
    } else {    // not quoted
        if(nameItem[0] == '"' || nameItem[strlen(nameItem) - 1] == '"') {
            return false;
        }
        strcpy(buffer, nameItem);    // otherwise copy original (unquoted)
    }

    return hasMatchingQuotes(buffer);
}

// ======================= LINKED LIST FUNCTIONS =======================
// prints out our entire linked list
void print_list(node_t* head) {
    node_t* current = head;

    while (current != NULL) {
        printf("%s has an occurrence of: %d\n", current->name, current->occurrences);
        current = current->next;
    }
}

// push a new linked list node in current linked list
void push(node_t** head, const char* name) {
    if(*head == NULL) {
        *head = malloc(sizeof(node_t));
        if(*head == NULL) {
            printf("%s", ERROR_MESSAGE);
            return;
        }

        (*head)->occurrences = 1;
        (*head)->name = (char*) malloc(sizeof(char) * (strlen(name) + 1));
        strcpy((*head)->name, name);

        if((*head)->name == NULL) {
            printf("%s", ERROR_MESSAGE);
            return;
        }
        (*head)->next = NULL;

        return;
    }

    node_t* current = *head;
    while (current != NULL) {
        //if name exists already
        // printf("cur name: %s | compared to name: %s\n", current->name, name);
        if(strcmp(current->name, name) == 0) {
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
    current->next->name = (char*) malloc(sizeof(char) * (strlen(name) + 1));
    if(!current->next->name) {
        printf("%s", ERROR_MESSAGE);
        return;
    }

    strcpy(current->next->name, name);
    current->next->occurrences = 1;
    current->next->next = NULL;
}

// removes the head of linked list
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

// removes linked list item by its index
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

// gets largest occurrence in linked list and prints it out
void getLargestOccurrence(node_t ** head) {
    node_t* current = *head;
    int cur_i = 0;
    int max = 0;
    int max_i = 0;
    char* max_name;

    while (current != NULL) {
        // printf("%s has an occurrence of: %d\n", current->name, current->occurrences);
        if(current->occurrences > max) {
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
    free(max_name);
    remove_by_index(head, max_i);
    printf("---removal complete\n");
}

// ========================== FREEING FUNCTIONS ==========================
// frees the linked list nodes
void freeLinkedList(node_t* head) {
   node_t* tmp;
   while (head != NULL) {
       tmp = head;
       head = head->next;
       free(tmp->name);
       free(tmp);
    }
}

// frees header names
void freeHeader(header_t headers[], int headerCount) {
    for(int i = 0; i < headerCount; ++i) {
        free(headers[i].headerName);
    }
}

// =============================  MAIN ====================================
int main(int argc, const char *argv[]) {
    char line[MAX_CHAR];    // line with max_char size
    int nameIndex;          // index of "name" line field
    header_t headers[MAX_CHAR]; // holds the header type (name and if quoted)
    int headerCount = 0;    // total header items
    int fileLineCount = 0;  // total lines in file
    node_t* head = NULL;    // head of linked list
    int colCount = 0;       // used to see how many columns after we get a line
    char buffer[MAX_CHAR];  // used to store field after taking away quotes

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

    fileLineCount++;

    // on successful fgets, the string is terminated with newline
    // if no newline, means buffer size exceeded
    if(line[strlen(line) - 1] != '\n') {
        printf("first overflow!!!\n");
        printf("Overflow detection!!!!!!\n");
        return errorMsg();
    }

    // for safety, intialize members in headers struct
    for(int i = 0; i < MAX_CHAR; ++i) {
        headers[i].headerName = NULL;
        headers[i].isQuoted = false;
    }

    trimLine(line);
    nameIndex = valAndGetNameIndex(line, headers, &headerCount);
    if(nameIndex == -1) {
        return errorMsg();
    }

    // printf("%d\n", nameIndex);

    // Part 3 - Get Unique Names
    // loop through entire file to collect number of names
    // while(fgets(line, MAX_CHAR, file)) {
        // printf("Name: %s\n", getField(line, nameIndex));
    // }

    line[MAX_CHAR - 1] = '\0';     // set last char of buffer to null character

    // TODO: need to check each nameIndex item is quoted or not
    // check if valid header but no content (considered valid)
    if(!fgets(line, MAX_CHAR, file)) {
        printf("valid header but no content\n");
        return 0;
    }

    // Part 4 - Create Hashtable
    // store names in hashtable
    // another loop to insert the names in hashtable

    // use do while loop because already have a fgets above
    do {
        fileLineCount++;
        if(fileLineCount > MAX_LINES) {  // make filed doesn't exceed MAX_LINES
            printf("FILE EXCEEDED MAX LINES\n");
            return errorMsg();
        }
        // must check for buffer overflow (can tell if null char got overwritten)
        if(line[MAX_CHAR - 1] != '\0') {
            printf("second overflow!!!!!!!\n");
            printf("Overflow detection!!!!!!\n");
            return errorMsg();
        }
        trimLine(line);
        const char* curNameItem = getField(line, nameIndex, &colCount);
        if(curNameItem) {
            if(!nameItemQuoteCorrect(curNameItem, headers[nameIndex].isQuoted, buffer)) {
                printf("name item inconsistent with header quote style\n");
                return errorMsg();
            }
            printf("bfore push\n");
            printf("curNameItem:%s\n",curNameItem);
            push(&head, buffer);
            printf("after push\n");
            if(colCount != headerCount) {
                printf("inconsistent header and column counts\n");
                return errorMsg();
            }
        }
        // printf("Name: %s\n", getField(line, nameIndex));
    } while(fgets(line, MAX_CHAR, file));

    // gets top 10 largest or as many nodes as possible from linked list
    for(int i = 0; i < 10 && head; ++i) {
        getLargestOccurrence(&head);
    }

    printf("linecount total:%d\n",fileLineCount);
    printf("Headr count:%d\n", headerCount);
    // printf("HAS MATCHING QUOTES: %d\n", hasMatchingQuotes("a\"b"));

    // TODO: FREE EVERYTHING ALLOCATED (headers and linkedlist nodes)
    freeHeader(headers, headerCount);
    freeLinkedList(head);

    return 0;
}
