
#ifndef BUFFERPOOL_H_
#define BUFFERPOOL_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define PAGE_SIZE 4096
#define PAGE_NUM 2

typedef enum{
    false = 0,
    true = 1
} bool;

extern FILE * fp;
extern FILE * fo;
extern char * buf;
extern char pcnt;

bool new_buf();
bool getPage();
void closeBuf();
void startLexer();


static inline char getPcnt() { pcnt = (pcnt == 0) ? 1 : 0;    return pcnt;}


#endif //BUFFERPOOL_H_