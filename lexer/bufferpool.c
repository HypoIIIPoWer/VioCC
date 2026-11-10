#include"bufferpool.h"

FILE * fp = NULL;
FILE * fo = NULL;
char * buf = NULL;
char pcnt = 1;

bool new_buf(){
    buf = (char*)malloc(sizeof(char) * PAGE_SIZE * PAGE_NUM + 1);
    if(buf == NULL){
        printf("cant malloc buffer!!!\n");
        return false;
    }

    fp = fopen("test.txt", "r");
    fo = fopen("log.txt", "w");
    if(fp == NULL || fo == NULL){
        printf("cant read or write file!!!\n");
        return false;
    }

    size_t read_num = fread(buf, sizeof(char), PAGE_SIZE * PAGE_NUM, fp);
    if(read_num == PAGE_SIZE * PAGE_NUM){
        buf[read_num] = '@';
    }else if(feof(fp)){
        buf[read_num] = '\0';
    }else if (ferror(fp)){
        printf("fread failed ,something's wrong\n");
        return false;
    }
    
    return true;
}

bool getPage(){
    size_t ofs = PAGE_SIZE * getPcnt();
    size_t read_num = fread(buf + ofs, sizeof(char), PAGE_SIZE, fp);
    if(read_num == PAGE_SIZE){
        buf[read_num + ofs] = '@';
    }else if(feof(fp)){
        buf[read_num + ofs] = '\0';
    }else if (ferror(fp)){
        printf("fread failed ,something's wrong\n");
        return false;
    }

    return true;
}

void closeBuf(){
    free(buf);
    buf = NULL;
    if(fp) { fclose(fp); fp = NULL; }
    if(fo) { fclose(fo); fo = NULL; }
}