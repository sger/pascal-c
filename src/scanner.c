#include "scanner.h"
#include "error.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define EOF_CHAR '\x7f'
#define TAB_SIZE 8
#define MAX_INTEGER 32767
#define MAX_DIGIT_COUNT 20
#define MAX_EXPONENT 37
#define MIN_RESERVED_WORD_LENGTH 2
#define MAX_RESERVED_WORD_LENGTH 9

typedef enum {
  LETTER,
  DIGIT,
  SPECIAL,
  EOF_CODE,
  QUOTE,
} CHAR_CODE;

typedef struct {
  char *string;
  TOKEN_CODE tokenCode;
} RW_STRUCT;

RW_STRUCT rw_2[] = {
    {"do", DO}, {"if", IF}, {"in", IN}, {"of", OF},
    {"or", OR}, {"to", TO}, {NULL, 0},
};

RW_STRUCT rw_3[] = {
    {"and", AND}, {"div", DIV}, {"end", END}, {"for", FOR}, {"mod", MOD},
    {"nil", NIL}, {"not", NOT}, {"set", SET}, {"var", VAR}, {NULL, 0},
};

RW_STRUCT rw_4[] = {
    {"case", CASE}, {"else", ELSE}, {"file", FFILE}, {"goto", GOTO},
    {"then", THEN}, {"type", TYPE}, {"with", WITH},  {NULL, 0},
};

RW_STRUCT rw_5[] = {
    {"array", ARRAY}, {"begin", BEGIN}, {"const", CONST}, {"label", LABEL},
    {"until", UNTIL}, {"while", WHILE}, {NULL, 0},
};

RW_STRUCT rw_6[] = {
    {"downto", DOWNTO}, {"packed", PACKED}, {"record", RECORD},
    {"repeat", REPEAT}, {NULL, 0},
};

RW_STRUCT rw_7[] = {
    {"program", PROGRAM},
    {NULL, 0},
};

RW_STRUCT rw_8[] = {
    {"function", FUNCTION},
    {NULL, 0},
};

RW_STRUCT rw_9[] = {
    {"procedure", PROCEDURE},
    {NULL, 0},
};

RW_STRUCT *rw_table[] = {
    NULL, NULL, rw_2, rw_3, rw_4, rw_5, rw_6, rw_7, rw_8, rw_9,
};

// Current input character
char currentCharacter;

TOKEN_CODE tokenCode;
LITERAL literal;
CHAR_CODE charTable[256];
int bufferOffset;
int lineNunber = 0;
int pageNumber = 0;
int level = 0;
int lineCount = MAX_LINES_PER_PAGE;
int digitCount;

bool printFlag = true;
bool countError;

char sourceBuffer[MAX_SOURCE_LINE_LENGTH];
char tokenString[MAX_TOKEN_STRING_LENGTH];
char sourceName[MAX_FILE_NAME_LENGTH];
char wordString[MAX_TOKEN_STRING_LENGTH];
char date[DATE_STRING_LENGTH];
char *bufferp = sourceBuffer;
char *tokenp = tokenString;

FILE *file;

#define charCode(currentCharacter) charTable[currentCharacter]

void initializeScanner(const char *name) {
  int i;
  for (i = 0; i < 256; ++i) {
    charTable[i] = SPECIAL;
  }

  for (i = '0'; i <= '9'; ++i) {
    charTable[i] = DIGIT;
  }

  for (i = 'A'; i <= 'Z'; ++i) {
    charTable[i] = LETTER;
  }

  for (i = 'a'; i <= 'z'; ++i) {
    charTable[i] = LETTER;
  }

  charTable['\''] = QUOTE;
  charTable[EOF_CHAR] = EOF_CODE;

  initializePageHeader(name);
  openSourceFile(name);
}

void initializePageHeader(const char *name) {
  time_t timer;
  strncpy(sourceName, name, MAX_FILE_NAME_LENGTH - 1);
  time(&timer);
  strcpy(date, asctime(localtime(&timer)));
}

void openSourceFile(const char *name) {
  if ((name == NULL) || ((file = fopen(name, "r")) == NULL)) {
    printf("Error: failed to open source file\n");
    error(FAILED_SOURCE_FILE_OPEN);
    exit(-FAILED_SOURCE_FILE_OPEN);
  }

  bufferp = "";
  getChar();
}

bool isReservedWord() {
  int wordLength = strlen(wordString);
  RW_STRUCT *rwp;

  if ((wordLength >= MIN_RESERVED_WORD_LENGTH) &&
      (wordLength <= MAX_RESERVED_WORD_LENGTH)) {

    for (rwp = rw_table[wordLength]; rwp->string != NULL; ++rwp) {
      if (strcmp(wordString, rwp->string) == 0) {
        tokenCode = rwp->tokenCode;
        return true;
      }
    }
  }

  return false;
}

void downshiftWord() {
  int offset = 'a' - 'A';
  char *wp = wordString;
  char *tp = tokenString;

  do {
    *wp++ = (*tp >= 'A') && (*tp <= 'Z') ? *tp + offset : *tp;
    ++tp;
  } while (*tp != '\0');

  *wp = '\0';
}

void getWord() {
  while ((charCode(currentCharacter) == LETTER) ||
         (charCode(currentCharacter) == DIGIT)) {
    *tokenp++ = currentCharacter;
    getChar();
  }

  *tokenp = '\0';
  downshiftWord();

  if (!isReservedWord()) {
    tokenCode = IDENTIFIER;
  }
}

void accumulateValue(float *valuep, ERROR_CODE errorCode) {
  float value = *valuep;

  if (charCode(currentCharacter) != DIGIT) {
    error(errorCode);
    tokenCode = ERROR;
    return;
  }

  do {
    *tokenp++ = currentCharacter;
    if (++digitCount <= MAX_DIGIT_COUNT) {
      value = 10 * value + (currentCharacter - '0');
    } else {
      countError = true;
    }
    getChar();
  } while (charCode(currentCharacter) == DIGIT);

  *valuep = value;
}

void getNumber() {
  int wholeCount = 0;
  int decimalOffset = 0;
  char exponentSign = '+';
  int exponent = 0;
  float nvalue = 0.0;
  float evalue = 0.0;
  bool sawDotdot = false;

  digitCount = 0;
  countError = false;
  tokenCode = NO_TOKEN;

  literal.type = INTEGER_LIT;

  accumulateValue(&nvalue, INVALID_NUMBER);

  if (tokenCode == ERROR) {
    return;
  }
  wholeCount = digitCount;

  if (currentCharacter == '.') {
    getChar();

    if (currentCharacter == '.') {
      sawDotdot = true;
      --bufferp;
    } else {
      literal.type = REAL_LIT;
      *tokenp++ = '.';

      accumulateValue(&nvalue, INVALID_FRACTION);
      if (tokenCode == ERROR) {
        return;
      }
      decimalOffset = wholeCount - digitCount;
    }
  }

  if (!sawDotdot && ((currentCharacter == 'E') || (currentCharacter == 'e'))) {
    literal.type = REAL_LIT;
    *tokenp++ = currentCharacter;
    getChar();

    if ((currentCharacter == '+') || (currentCharacter == '-')) {
      *tokenp++ = exponentSign = currentCharacter;
      getChar();
    }

    accumulateValue(&evalue, INVALID_EXPONENT);
    if (tokenCode == ERROR) {
      return;
    }
    if (exponentSign == '-') {
      evalue = -evalue;
    }
  }

  if (countError) {
    error(TOO_MANY_DIGITS);
    tokenCode = ERROR;
    return;
  }

  exponent = evalue + decimalOffset;
  if ((exponent + wholeCount < -MAX_EXPONENT) ||
      (exponent + wholeCount > MAX_EXPONENT)) {
    error(REAL_OUT_OF_RANGE);
    tokenCode = ERROR;
    return;
  }

  if (exponent != 0) {
    nvalue *= pow(10, exponent);
  }

  if (literal.type == INTEGER_LIT) {
    if ((nvalue < -MAX_INTEGER) || (nvalue > MAX_INTEGER)) {
      error(INTEGER_OUT_OF_RANGE);
      tokenCode = ERROR;
      return;
    }
    literal.value.integer = nvalue;
  } else {
    literal.value.real = nvalue;
  }

  *tokenp = '\0';
  tokenCode = NUMBER;
}

void getSpecial() {
  *tokenp++ = currentCharacter;
  switch (currentCharacter) {
  case '^':
    tokenCode = UPARROW;
    getChar();
    break;
  case '*':
    tokenCode = STAR;
    getChar();
    break;
  case '(':
    tokenCode = LPAREN;
    getChar();
    break;
  case ')':
    tokenCode = RPAREN;
    getChar();
    break;
  case '-':
    tokenCode = MINUS;
    getChar();
    break;
  case '+':
    tokenCode = PLUS;
    getChar();
    break;
  case '=':
    tokenCode = EQUAL;
    getChar();
    break;
  case '[':
    tokenCode = LBRACKET;
    getChar();
    break;
  case ']':
    tokenCode = RBRACKET;
    getChar();
    break;
  case ';':
    tokenCode = SEMICOLON;
    getChar();
    break;
  case ',':
    tokenCode = COMMA;
    getChar();
    break;
  case '/':
    tokenCode = SLASH;
    getChar();
    break;
  case ':':
    getChar();
    if (currentCharacter == '=') {
      *tokenp++ = '=';
      tokenCode = COLONEQUAL;
      getChar();
    } else {
      tokenCode = COLON;
    }
    break;
  case '<':
    getChar();
    if (currentCharacter == '=') {
      *tokenp++ = '=';
      tokenCode = LE;
      getChar();
    } else if (currentCharacter == '>') {
      *tokenp++ = '>';
      tokenCode = NE;
      getChar();
    } else {
      tokenCode = LT;
    }
    break;
  case '>':
    getChar();
    if (currentCharacter == '=') {
      *tokenp++ = '=';
      tokenCode = GE;
      getChar();
    } else {
      tokenCode = GT;
    }
    break;
  case '.':
    getChar();
    if (currentCharacter == '.') {
      *tokenp++ = '.';
      tokenCode = DOTDOT;
      getChar();
    } else {
      tokenCode = PERIOD;
    }
    break;
  default:
    tokenCode = ERROR;
    getChar();
    break;
  }
  *tokenp = '\0';
}

void getString() {
  char *sp = literal.value.string;

  *tokenp++ = '\'';
  getChar();

  while (currentCharacter != EOF_CHAR) {
    if (currentCharacter = '\'') {
      *tokenp++ = currentCharacter;
      getChar();
      if (currentCharacter != '\'') {
        break;
      }
      *tokenp++ = currentCharacter;
      *sp++ = currentCharacter;
      getChar();
    }

    *tokenp = '\0';
    *sp = '\0';
    tokenCode = STRING;
    literal.type = STRING_LIT;
  }
}

void getToken() {
  while (currentCharacter == ' ') {
    getChar();
  }
  tokenp = tokenString;

  switch (charCode(currentCharacter)) {
  case LETTER:
    getWord();
    break;
  case DIGIT:
    getNumber();
    break;
  case QUOTE:
    getString();
    break;
  case EOF_CODE:
    tokenCode = END_OF_FILE;
    break;
  default:
    getSpecial();
    break;
  }
}

void quitScanner() { fclose(file); }

bool getSourceLine() {
  char printBuffer[MAX_SOURCE_LINE_LENGTH + 9];
  if ((fgets(sourceBuffer, MAX_SOURCE_LINE_LENGTH, file)) != NULL) {
    ++lineNunber;
    if (printFlag) {
      sprintf(printBuffer, "%4d %d: %s", lineNunber, level, sourceBuffer);
      printLine(printBuffer);
    }
    return true;
  } else {
    return false;
  }
}

void getChar() {
  if (*bufferp == '\0') {
    if (!getSourceLine()) {
      currentCharacter = EOF_CHAR;
      return;
    }
    bufferp = sourceBuffer;
    bufferOffset = 0;
  }

  currentCharacter = *bufferp++;

  switch (currentCharacter) {
  case '\t':
    bufferOffset += TAB_SIZE - bufferOffset % TAB_SIZE;
    currentCharacter = ' ';
    break;
  case '\n':
    ++bufferOffset;
    currentCharacter = ' ';
    break;
  case '{':
    ++bufferOffset;
    skipComment();
    currentCharacter = ' ';
    break;

  default:
    ++bufferOffset;
  }
}

void skipComment() {
  do {
    getChar();
  } while ((currentCharacter != '}') && (currentCharacter != EOF_CHAR));
}

void printPageHeader() {
  putchar(FORM_FEED_CHAR);
  printf("Page %d %s %s\n\n", ++pageNumber, sourceName, date);
}

void printLine(char line[]) {
  char saveCharacter;
  char *saveCharacterp = NULL;

  if (++lineCount > MAX_LINES_PER_PAGE) {
    printPageHeader();
    lineCount = 1;
  }

  if (strlen(line) > MAX_PRINT_LINE_LENGTH) {
    saveCharacterp = &line[MAX_SOURCE_LINE_LENGTH];
    saveCharacter = *saveCharacterp;
    *saveCharacterp = '\0';
  }

  printf("Line %s", line);

  if (saveCharacterp) {
    *saveCharacterp = saveCharacter;
  }
}