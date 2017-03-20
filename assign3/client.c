#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "record_mgr.h"
#include "dberror.h"
#include "const.h"

#define MAX_NAME 256
#define MAX_NUM_ATTR 16
#define MAX_STRING 7000
#define MAX_NAME_ATTEMPTS 5


char *copyStr(char *_string) {
  char *string = newStr(strlen(_string)); // TODO free it[count, multiple maybe]
  strcpy(string, _string);
  return string;
}


void showError(char *msg) {
  printf("ERROR! : %s!\n", msg);
}

int validateName(char *str) {
  str[0] = tolower(str[0]);
  if (str[0] < 97 || str[0] > 122) {
    return -1;
  }
  for(int i = 1; str[i]; i++) {
    if (i >= MAX_NAME - 1) {
      return -1;
    }
    str[i] = tolower(str[i]);
    if (str[i] == ',') {
      return -1;
    }
  }
  return 0;
}

void getName(char *token) {
  while (true) {
    scanf("%s", token);
    if (validateName(token) == -1) {
      showError("NAMES MUST NOT EXCEED THE MAX! NAMES MUST START WITH LETTER! NAMES MUST NOT CONTAIN `,`!");
      printf("Please try again : ");
      continue;
    }
    break;
  }
}


void processCommand(char *input) {
  RC err;
  if (strcmp("CT", input) == 0) {
    char *token = newStr(PAGE_SIZE);
    printf("Enter table name! (MAX_LEN=%d) : ", MAX_NAME - 1); // -1 for \0 terminator
    getName(token);
    char *tableName = copyStr(token);
    printf("Enter number of attributes! (MAX=%d) : ", MAX_NUM_ATTR);
    scanf("%s", token);
    int numAttrs = (int) strtol(token, NULL, 10);
    if (numAttrs > MAX_NUM_ATTR) {
      return showError("MAX EXCEEDED");
    }
    char **attrNames = newArray(char *, numAttrs);
    DataType *dataTypes = newArray(DataType, numAttrs);
    int *typeLength = newIntArr(numAttrs);
    int i;
    DataType dt;
    int dtlen;
    for (i = 0; i < numAttrs; i++) {
      printf("Building attribute#%d!\n", i);
      printf("\tEnter attribute#%d name! (MAX_LEN=%d) : ", i, MAX_NAME - 1);
      getName(token);
      attrNames[i] = copyStr(token);
      printf("\tEnter attribute#%d type! [0 : INT, 1 : STRING, 2 : FLOAT, 3 : BOOL] : ", i);
      scanf("%s", token);
      dt = (DataType) strtol(token, NULL, 10);
      if (dt != 0 && dt != 1 && dt != 2 && dt != 3) {
        return showError("UNKNOWN DATA TYPE");
      }
      dataTypes[i] = dt;
      if (dt == 1) {
        printf("\tEnter attribute#%d length! (MAX=%d) : ", i, MAX_STRING);
        scanf("%s", token);
        dtlen = (int) strtol(token, NULL, 10);
        if (dtlen > MAX_STRING) {
          return showError("MAX EXCEEDED");
        }
        typeLength[i] = dtlen;
      }
      else {
        typeLength[i] = 0;
      }
    }
    int *keys = newArray(int, 1);
    keys[0] = 0;
    Schema *schema = createSchema(numAttrs, attrNames, dataTypes, typeLength, 1, keys);
    err = createTable (tableName, schema);
    if (err) {
      printf("%d\n", err);
      return showError("SOMETHING WENT WRONG");
    }
    printf("Table `%s` created!\n", tableName);
  }
  else if (strcmp(input, "IR") == 0) {
    // TODO
  }
  else {
    showError("UNKNOWN COMMAND");
  }
}


int main(int argc, char *argv[]) {
  initRecordManager(NULL);
  char *input = newStr(PAGE_SIZE);
  while(1) {
    printf("Enter your command! [CT : createTable, IR : insertRecord, DR : deleteRecord, UR : updateRecord, ST : scanTable, DT : deleteTable] : ");
    scanf("%s", input);
    processCommand(input);
  }
}