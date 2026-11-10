#include"lexer.h" 

char *cursor = NULL;
size_t crow = 0;
size_t ccol = 0;

const char * keywords[] = {
    "program",
    "const",
    "var",
    "procedure",
    "begin",
    "end",
    "if",
    "then",
    "else",
    "while",
    "do",
    "call",
    "read",
    "write",
    "odd"
};

#define KEYWORD_NUM 15 //lol counted by my eyes :)

const char symbols[] = {
    ';' , ':' , '=' , '<' , '>' , '+' , '-' , '*' , '/' , ',' , '(' , ')' , '\0'
}; //not used , just for the dev to look up

bool is_keyword(char * id){
    for(int i = 0; i < KEYWORD_NUM; i++){
        if(strcmp(id, keywords[i]) == 0){
            return true;
        }
    }
    return false;
}

bool new_lexer(){
    if(!new_buf()) return false;
    cursor = buf;
    crow = 1;
    ccol = 1;
    return true;
}

token getToken(){
    token tok = {0};
    if(*cursor == '\0'){
        tok.type = TOKEN_FILE_END;
        tok.row = crow;
        tok.col = ccol;
        strcpy(tok.data, "The fucking file ends!!!");
    }else if(*cursor == '\n'){
    tok.type = TOKEN_SKIP;
    crow++;
    ccol = 1;
    tok.data[0] = '\\';
    tok.data[1] = 'n';
    tok.data[2] = '\0';

    cursor++;
}else if(*cursor == '\r'){
    tok.type = TOKEN_SKIP;
    tok.data[0] = '\\';
    tok.data[1] = 'r';
    tok.data[2] = '\0';

    cursor++;            
}else if(*cursor == '\t'){
    tok.type = TOKEN_SKIP;
    ccol++;
    tok.data[0] = '\\';
    tok.data[1] = 't';
    tok.data[2] = '\0';

    cursor++;
}else if (*cursor == ' '){
        tok.type = TOKEN_SKIP;
        ccol++;
        strcpy(tok.data, "\" \"");

        cursor++;
    }else if(*cursor == '+' || *cursor == '-' || *cursor == '*' || *cursor == '/' || *cursor == ',' || *cursor == '(' || *cursor == ')' || *cursor == ';' || *cursor == '='){
        tok.type = TOKEN_SYMBOL;
        tok.row = crow;
        tok.col = ccol;
        ccol++;
        tok.data[0] = *cursor;
        tok.data[1] = '\0';

        cursor++;
    }else if(*cursor == ':'){
        if(*(cursor + 1) == '='){
            tok.type = TOKEN_SYMBOL;
            tok.row = crow;
            tok.col = ccol;
            ccol += 2;
            tok.data[0] = *cursor;
            tok.data[1] = *(cursor + 1);
            tok.data[2] = '\0';

            cursor += 2;
        }else{
            tok.type = TOKEN_ERROR;
            tok.row = crow;
            tok.col = ccol;
            ccol++;
            strcpy(tok.data, "no \"=\" after \":\"");

            cursor++;
        }
    }else if(*cursor == '>' || *cursor == '<'){
        if(*(cursor + 1) == '='){
            tok.type = TOKEN_SYMBOL;
            tok.row = crow;
            tok.col = ccol;
            ccol += 2;
            tok.data[0] = *cursor;
            tok.data[1] = '=';
            tok.data[2] = '\0';

            cursor += 2;
        }else if(*cursor == '<' && *(cursor + 1) == '>'){
            tok.type = TOKEN_SYMBOL;
            tok.row = crow;
            tok.col = ccol;
            ccol += 2;
            tok.data[0] = *cursor;
            tok.data[1] = '>';
            tok.data[2] = '\0';

            cursor += 2;
        }else{
            tok.type = TOKEN_SYMBOL;
            tok.row = crow;
            tok.col = ccol;
            ccol++;
            tok.data[0] = *cursor;
            tok.data[1] = '\0';

            cursor++;
        }
    }else if(is_char(*cursor)){
        tok.row = crow;
        tok.col = ccol;
        size_t len = 0;
        while (is_char(cursor[len]) || is_digital(cursor[len])){
            tok.data[len] = cursor[len];
            len++;
        }
        tok.data[len] = '\0';
        if(is_keyword(tok.data)) tok.type = TOKEN_KEYWORD;
        else tok.type = TOKEN_ID;
        ccol += len;

        cursor += len;
    }else if(is_digital(*cursor)){
        tok.type = TOKEN_DIGITAL;
        tok.row = crow;
        tok.col = ccol;
        size_t len = 0;
        while (is_digital(cursor[len])){
            tok.data[len] = cursor[len];
            len++;
        }
        tok.data[len] = '\0';
        ccol += len;

        cursor += len;
    }else if(*cursor == '@'){
        tok.type = TOKEN_SKIP;
        tok.row = crow;
        tok.col = ccol;
        ccol++;
        getPage();
        tok.data[0] = *cursor;
        tok.data[1] = '\0';

        cursor = buf + PAGE_SIZE * pcnt;
    }else{
        tok.type = TOKEN_ERROR;
        tok.row = crow;
        tok.col = ccol;
        ccol++;
        strcpy(tok.data, "WHAT THE HELL IS THIS CHAR?!!");

        cursor++;
    }
    

    return tok;
}

void printHead(){
    fprintf(fo, "row\tcol\tToken_Type\tcontent\n");
}

void printToken_Type(){
    fprintf(fo, "TOKEN_ERROR = -1\nTOKEN_FILE_END = 0\nTOKEN_SKIP = 1\nTOKEN_KEYWORD = 2\nTOKEN_SYMBOL = 3\nTOKEN_ID = 4\nTOKEN_DIGITAL = 5\nNEED_PAGE = 6\n\n\n\n-------------------------------------------------------\n\n\n\n");
}

void printToken(token tok){
    fprintf(fo, "%zu\t%zu\t%d\t        %s\n", tok.row, tok.col, tok.type, tok.data);
}

void startLexer(){
    if(!new_lexer()){
        closeLexer();
        printf("Lexer cant initiate!\n");
        return;
    }
    token tok = {0};
    printToken_Type();
    printHead();
    do{
        tok = getToken();
        printToken(tok);
    }while(tok.type != TOKEN_FILE_END);
    

    closeLexer();
}

void closeLexer(){
    closeBuf();
}