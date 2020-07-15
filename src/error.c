#include "error.h"
#include "common.h"
#include <stdbool.h>

extern char *tokenp;
extern bool print_flag;
extern char source_buffer[];
extern char *bufferp;

char *errorMessages[] = {
    "No error",
    "Syntax error",
    "Too many syntax errors",
    "Failed to open source file",
    "Unexpected end of file",
    "Invalid number",
    "Invalid fraction",
    "Invalid exponent",
    "Too many digits",
    "Real literal out of range",
    "Integer literal out of range",
    "Missing right parenthesis",
    "Invalid expression",
    "Invalid assignment statement",
    "Missing identifier",
    "Missing := ",
    "Undefined identifier",
    "Stack overflow",
    "Invalid statement",
    "Unexpected token",
    "Missing ; ",
    "Missing DO",
    "Missing UNTIL",
    "Missing THEN",
    "Invalid FOR control variable",
    "Missing OF",
    "Invalid constant",
    "Missing constant",
    "Missing : ",
    "Missing END",
    "Missing TO or DOWNTO",
    "Redefined identifier",
    "Missing = ",
    "Invalid type",
    "Not a type identifier",
    "Invalid subrangetype",
    "Not a constant identifier",
    "Missing .. ",
    "Incompatible types",
    "Invalid assignment target",
    "Invalid identifier usage",
    "Incompatible assignment",
    "Min limit greater than max limit",
    "Missing [ ",
    "Missing ] ",
    "Invalid index type",
    "Missing BEGIN",
    "Missing period",
    "Too many subscripts",
    "Invalid field",
    "Nesting too deep",
    "Missing PROGRAM",
    "Already specified in FORWARD",
    "Wrong number of actual parameters",
    "Invalid VAR parameter",
    "Not a record variable",
    "Missing variable",
    "Code segment overflow",
    "Unimplemented feature",
};

int errorCount = 0;

void error(ERROR_CODE errorCode) {
  extern int buffer_offset;
  char messageBuffer[MAX_PRINT_LINE_LENGTH];
  char *message = errorMessages[errorCode];
}