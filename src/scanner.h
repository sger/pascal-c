#ifndef scanner_h
#define scanner_h

#include "common.h"

typedef enum {
  NO_TOKEN,
  IDENTIFIER,
  NUMBER,
  STRING,
  UPARROW,
  STAR,
  LPAREN,
  RPAREN,
  MINUS,
  PLUS,
  EQUAL,
  LBRACKET,
  RBRACKET,
  COLON,
  SEMICOLON,
  LT,
  GT,
  COMMA,
  PERIOD,
  SLASH,
  COLONEQUAL,
  LE,
  GE,
  NE,
  DOTDOT,
  END_OF_FILE,
  ERROR,
  AND,
  ARRAY,
  BEGIN,
  CASE,
  CONST,
  DIV,
  DO,
  DOWNTO,
  ELSE,
  END,
  FFILE,
  FOR,
  FUNCTION,
  GOTO,
  IF,
  IN,
  LABEL,
  MOD,
  NIL,
  NOT,
  OF,
  OR,
  PACKED,
  PROCEDURE,
  PROGRAM,
  RECORD,
  REPEAT,
  SET,
  THEN,
  TO,
  TYPE,
  UNTIL,
  VAR,
  WHILE,
  WITH,
} TOKEN_CODE;

typedef enum {
  INTEGER_LIT,
  STRING_LIT,
  REAL_LIT,
} LITERAL_TYPE;

typedef struct {
  LITERAL_TYPE type;
  union {
    int integer;
    char string[MAX_SOURCE_LINE_LENGTH];
    float real;
  } value;
} LITERAL;

void initializeScanner(const char *name);
void initializePageHeader(const char *name);
void openSourceFile(const char *name);
void printLine(char line[]);
void printPageHeader();
void getChar();
void getToken();
void quitScanner();
void skipComment();

#endif