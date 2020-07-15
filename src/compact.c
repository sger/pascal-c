#include "common.h"
#include "scanner.h"
#include <stdbool.h>
#include <stdio.h>

#define MAX_OUTPUT_RECORD_LENGTH 80

typedef enum {
  DELIMITER,
  NONDELIMITER,
} TOKEN_CLASS;

extern TOKEN_CODE tokenCode;
extern char tokenString[];
extern bool printFlag;

int recordLength;
char *recp;

char outputRecord[MAX_OUTPUT_RECORD_LENGTH];

TOKEN_CLASS tokenClass() {
  switch (tokenCode) {
  case IDENTIFIER:
  case NUMBER:
    return NONDELIMITER;
  default:
    return tokenCode < AND ? DELIMITER : NONDELIMITER;
  }
}

void flushOutputRecord() {
  printf("%s\n", outputRecord);
  recp = outputRecord;
  *recp = '\0';
  recordLength = 0;
}

void appendBlank() {
  if (++recordLength == MAX_OUTPUT_RECORD_LENGTH - 1) {
    flushOutputRecord();
  } else {
    strcat(outputRecord, " ");
  }
}

void appendToken() {
  int tokenLength;
  tokenLength = strlen(tokenString);
  if (recordLength + tokenLength >= MAX_OUTPUT_RECORD_LENGTH - 1) {
    flushOutputRecord();
  }
  strcat(outputRecord, tokenString);
  recordLength += tokenLength;
}

int main2(int argc, const char *argv[]) {
  TOKEN_CLASS class;
  TOKEN_CLASS previousClass;
  TOKEN_CLASS tokenClass();

  printFlag = false;

  initializeScanner(argv[1]);

  previousClass = DELIMITER;
  recp = outputRecord;
  *recp = '\0';
  recordLength = 0;

  do {
    getToken();
    if (tokenCode == END_OF_FILE) {
      break;
    }
    class = tokenClass();

    if ((previousClass == NONDELIMITER) && (class == NONDELIMITER)) {
      appendBlank();
      appendToken();
      previousClass = class;
    }

  } while (tokenCode != PERIOD);

  if (recordLength > 0) {
    flushOutputRecord();
  }

  quitScanner();
}