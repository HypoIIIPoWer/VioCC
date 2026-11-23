#ifndef PARSER_H_
#define PARSER_H_

#include"lexer.h"
#define CHUNK_SIZE 65536

typedef struct chunk chunk;
typedef struct ASTNode ASTNode;
typedef struct Arena Arena;

struct chunk{
    chunk * next;
    size_t  used;
    char    data[];
};

struct Arena{
    size_t  chunknum;
    chunk * fisrt;
};

struct ASTNode{
    token     tok;
    ASTNode * childs[4];
};

extern token peeker[2];
extern size_t peekercnt;
extern size_t peekerofs;
extern Arena * arena;
extern chunk * chunkptr;

void * arenaMalloc(int n); //bytes
void resetArena();
void freeArena();

void movePeeker();
bool skipToken(); // if skipped at least one token , return true , else return false
token wrapParserErrorToken();
token wrapParserWarnToken();
token wrapNotATerminalToken();
token wrapTerminalToken(const char * s);

bool new_parser();
void startParser();
void closeParser();

bool checkToken(Token_Type type,const char * s);

ASTNode * parseProg();
ASTNode * parseBlock();
ASTNode * parseCondcel();
ASTNode * parseConst();
ASTNode * parseVardecl();
ASTNode * parseProc();
ASTNode * parseBody(); 
ASTNode * parseStatement();
ASTNode * parseLexp();
ASTNode * parseExp();
ASTNode * parseTerm();
ASTNode * parseFactor();
ASTNode * parseLop();
ASTNode * parseAop();
ASTNode * parseMop();
ASTNode * parseId();
ASTNode * parseInteger();

void printASTtree(ASTNode * node);


#endif // PARSER_H_