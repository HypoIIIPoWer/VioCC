
#ifndef LEXER_H_
#define LEXER_H_

#include"bufferpool.h"


typedef enum{
    TOKEN_ERROR = -1,
    TOKEN_FILE_END = 0,
    TOKEN_SKIP,
    TOKEN_KEYWORD,
    TOKEN_SYMBOL,
    TOKEN_ID,
    TOKEN_DIGITAL,
    NEED_PAGE
} Token_Type;

typedef struct{
    Token_Type type;
    char data[64];
    size_t row;
    size_t col;
} token;

extern char * cursor;
extern size_t crow;
extern size_t ccol;

bool new_lexer();
token getToken();
void closeLexer();
bool is_keyword(char * id);
void printToken(token tok);
void printHead();
void printToken_Type();

static inline bool is_char(char c){
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline bool is_digital(char c){
    return (c >= '0' && c <= '9');
}

#endif // LEXER_H_