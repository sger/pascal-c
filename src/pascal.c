#include "common.h"
#include "error.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *symbolStrings[] = {
    "<no token>", "<IDENTIFIER>",
    "<NUMBER>",   "<STRING>",
    "^",          "*",
    "(",          ")",
    "-",          "+",
    "=",          "[",
    "]",          ":",
    ";",          "<",
    ">",          ",",
    ".",          "/",
    ":=",         "<=",
    ">=",         "<>",
    "..",         "<END OF FILE>",
    "<ERROR>",    "AND",
    "ARRAY",      "BEGIN",
    "CASE",       "CONST",
    "DIV",        "DO",
    "DOWNTO",     "ELSE",
    "END",        "FILE",
    "FOR",        "FUNCTION",
    "GOTO",       "IF",
    "IN",         "LABEL",
    "MOD",        "NIL",
    "NOT",        "OF",
    "OR",         "PACKED",
    "PROCEDURE",  "PROGRAM",
    "RECORD",     "REPEAT",
    "SET",        "THEN",
    "TO",         "TYPE",
    "UNTIL",      "VAR",
    "WHILE",      "WITH",
};

extern TOKEN_CODE tokenCode;
extern char tokenString[];
extern LITERAL literal;

void printToken() {
  char line[MAX_SOURCE_LINE_LENGTH + 32];
  char *symbolString = symbolStrings[tokenCode];

  switch (tokenCode) {
  case NUMBER:
    if (literal.type == INTEGER_LIT) {
      sprintf(line, "     >> %-16s %d (integer)\n", symbolString,
              literal.value.integer);
    } else {
      sprintf(line, "     >> %-16s %g (real)\n", symbolString,
              literal.value.real);
    }
    break;
  case STRING:
    sprintf(line, "     >> %-16s %-s\n", symbolString, literal.value.string);
    break;
  default:
    sprintf(line, "     >> %-16s %-s\n", symbolString, tokenString);
    break;
  }
  printLine(line);
}

int main(int argc, const char *argv[]) {
  initializeScanner(argv[1]);

  do {
    getToken();
    if (tokenCode == END_OF_FILE) {
      error(UNEXPECTED_END_OF_FILE);
      break;
    }

    printToken();

  } while (tokenCode != PERIOD);

  quitScanner();
}