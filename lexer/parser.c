#include"parser.h"

token peeker[2] = {0};
size_t peekercnt = 0;
size_t peekerofs = 1;
Arena * arena = NULL;
chunk * chunkptr = NULL;

bool new_parser(){
    startLexer(false); //start without close
    if(!resetLexer()){
        printf("can't reset lexer!!!\n");
        return false;
    }
    peeker[0] = getToken();
    peeker[1] = getToken(); // well , don't give me a fucking shit program with less than 2 tokens ok?

    arena = (Arena*)malloc(sizeof(Arena));
    arena->chunknum = 1;
    arena->fisrt = (chunk*)malloc(sizeof(chunk) + CHUNK_SIZE);
    memset(arena->fisrt, 0, sizeof(chunk) + CHUNK_SIZE);
    chunkptr = arena->fisrt;
    chunkptr->next = NULL;
    chunkptr->used = 0;

    return true;
}

void * arenaMalloc(int n){
    n = (n + 7) & ~(size_t)7;   // n round up to the nearest multiple of 8

    if(chunkptr->used + n >= CHUNK_SIZE){
        chunkptr->next = (chunk*)malloc(sizeof(chunk) + CHUNK_SIZE);
        memset(chunkptr->next, 0, sizeof(chunk) + CHUNK_SIZE);
        arena->chunknum++;
        chunkptr = chunkptr->next;
        chunkptr->used = 0;
        chunkptr->next = NULL;
    }   //if size overflow chunk , malloc another

    char * p = chunkptr->data + chunkptr->used;
    chunkptr->used += n;
    chunkptr->used = (chunkptr->used + 7) & ~(size_t)7;
    return p;
}

void resetArena(){
    chunk * thischunk = arena->fisrt;
    chunk * nextchunk = arena->fisrt->next;
    while(1){
        free(thischunk);
        if(nextchunk == NULL) {
            break;
        }
        thischunk = nextchunk;
        nextchunk = nextchunk->next;
    }
    chunkptr = arena->fisrt;
    arena->chunknum = 1;
    arena->fisrt = (chunk*)malloc(sizeof(chunk) + CHUNK_SIZE);
    memset(arena->fisrt, 0, sizeof(chunk) + CHUNK_SIZE);
    chunkptr = arena->fisrt;
    chunkptr->next = NULL;
    chunkptr->used = 0;

}

void freeArena(){
    chunk * thischunk = arena->fisrt;
    chunk * nextchunk = arena->fisrt->next;
    while(1){
        free(thischunk);
        if(nextchunk == NULL) {
            break;
        }
        thischunk = nextchunk;
        nextchunk = nextchunk->next;
    }
    free(arena);
    arena = NULL;
    chunkptr = NULL;
}

void startParser(){
    if(!new_parser()){
        printf("cant start parser!!!\n");
        return;
    }
    ASTNode * RootNode = parseProg();
    printASTtree(RootNode);
    closeParser();
}

void closeParser(){
    closeLexer();
    free(arena);
    arena = NULL;
}

void movePeeker(){
    peeker[peekercnt] = getToken();
    peekercnt = (peekercnt == 1) ? 0 : 1;
    peekerofs++;
}

bool skipToken(){
    if(peeker[peekercnt].type != TOKEN_SKIP){
        return false;
    }
    while(peeker[peekercnt].type == TOKEN_SKIP){
        movePeeker();
    }
    return true;
}

token wrapParserErrorToken(){
    token tok = {0};
    tok.type = TOKEN_PARSER_ERROR;
    tok.row = peeker[peekercnt].row;
    tok.col = peeker[peekercnt].col;
    return tok;
}

token wrapParserWarnToken(){
    token tok = {0};
    tok.type = TOKEN_PARSER_WARN;
    tok.row = peeker[peekercnt].row;
    tok.col = peeker[peekercnt].col;
    return tok;
}

token wrapNotATerminalToken(){
    token tok = {0};
    tok.type = NOT_A_TERMINAL;
    tok.row = peeker[peekercnt].row;
    tok.col = peeker[peekercnt].col;
    return tok;
}

token wrapTerminalToken(const char * s){
    token tok = {0};
    tok.type = TERMINAL;
    tok.row = peeker[peekercnt].row;
    tok.col = peeker[peekercnt].col;
    strcpy(tok.data, s);
    return tok;
}

bool checkToken(Token_Type type,const char * s){
    if(peeker[peekercnt].type != type){
        return false;
    }
    if(s != NULL && strcmp(s, peeker[peekercnt].data) != 0){
        return false;
    }

    return true;
}

ASTNode * parseProg(){
    if(checkToken(TOKEN_KEYWORD, "program") == false){
        fprintf(fo, "where's \"program\" at the beginning?\n");
    }
    ASTNode * prog = {(ASTNode*)arenaMalloc(sizeof(ASTNode))};
    prog->tok = peeker[peekercnt];
    prog->tok.type = TERMINAL;
    movePeeker();
    skipToken();
    if(!checkToken(TOKEN_ID, NULL)){
        fprintf(fo, "need ID after keyword program\n");
    }else{
    prog->childs[0] = parseId();
    }
    if(checkToken(TOKEN_SYMBOL, ";")){
        movePeeker();
    }else{
        fprintf(fo, "missed \";\" after program id\n");
    }
    skipToken();
    prog->childs[1] = parseBlock();

    return prog;
}

ASTNode * parseBlock(){
    ASTNode * block = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    block->tok.type = NOT_A_TERMINAL;
    skipToken();
    if(checkToken(TOKEN_KEYWORD, "const")){
        block->childs[0] = parseCondcel();
    }
    skipToken();
    if(checkToken(TOKEN_KEYWORD, "var")){
        block->childs[1] = parseVardecl();
    }
    skipToken();
    if(checkToken(TOKEN_KEYWORD, "procedure")){
        block->childs[2] = parseProc();
    }
    skipToken();
    if(!checkToken(TOKEN_KEYWORD, "begin")){
        fprintf(fo ,"need keyword \"begin\" at the beginning of a body\n");
        block->tok = wrapParserErrorToken();
        return block;
    }
    block->childs[3] = parseBody();

    return block;
}

ASTNode * parseCondcel(){
    ASTNode * condcel = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    condcel->tok.type = NOT_A_TERMINAL;
    skipToken();
    if(!checkToken(TOKEN_KEYWORD, "const")){
        fprintf(fo, "not a condcel\n");
        condcel->tok = wrapParserErrorToken();
    }else{
    condcel->childs[0] = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    condcel->childs[0]->tok = wrapTerminalToken("const");
    }
    movePeeker();
    skipToken();
    ASTNode * consttracker = condcel->childs[3];
    do{
        consttracker = (ASTNode*)arenaMalloc(sizeof(ASTNode));
        consttracker = parseConst();
        skipToken();
        if(!checkToken(TOKEN_SYMBOL, ",")){
            break;
        }
        movePeeker();
        skipToken();
        consttracker = consttracker->childs[3];
    }while(1); // loop parse const , downgrade tree into a linklist (what a stupid solution :( )
    if(!checkToken(TOKEN_SYMBOL, ";")){
        fprintf(fo, "need \";\" after a const declaration\n");
        condcel->tok = wrapParserWarnToken();
    }
    return condcel;
}

ASTNode * parseConst(){
    ASTNode * myconst = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    myconst->tok.type = NOT_A_TERMINAL;
    skipToken();
    if(!checkToken(TOKEN_ID, NULL)){
        fprintf(fo, "need a Id in const declare\n");
        myconst->tok = wrapParserErrorToken();
        return myconst;
    }
    myconst->childs[0] = parseId();
    skipToken();
    if(!checkToken(TOKEN_SYMBOL, ":=")){
        fprintf(fo, "need \":=\" after Id\n");
        myconst->tok = wrapParserErrorToken();
        return myconst;
    }
    myconst->childs[1] = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    myconst->childs[1]->tok = peeker[peekercnt];
    movePeeker();
    skipToken();
    if(!checkToken(TOKEN_DIGITAL, NULL)){
        fprintf(fo, "need Integer after :=\n");
        myconst->tok = wrapParserErrorToken();
        return myconst;
    }
    myconst->childs[2] = parseInteger();

    return myconst;
}

ASTNode * parseVardecl(){
    ASTNode * vardecl = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    vardecl->tok.type = NOT_A_TERMINAL;
    skipToken();
    if(!checkToken(TOKEN_KEYWORD, "var")){
        fprintf(fo, "not a vardecl\n");
        vardecl->tok = wrapParserErrorToken();
    }else{
        vardecl->childs[0] = (ASTNode*)arenaMalloc(sizeof(ASTNode));
        vardecl->childs[0]->tok = wrapTerminalToken("var");
        movePeeker();
    }
    skipToken();
    ASTNode * vardecltracker = vardecl->childs[3];
    do{
        vardecltracker = (ASTNode*)arenaMalloc(sizeof(ASTNode));
        vardecltracker = parseId();
        skipToken();
        if(!checkToken(TOKEN_SYMBOL, ",")){
            break;
        }
        movePeeker();
        skipToken();
        vardecltracker = vardecltracker->childs[3];
    }while(1);
    if(!checkToken(TOKEN_SYMBOL, ";")){
        fprintf(fo, "need \";\" after a var declaration\n");
        vardecl->tok = wrapParserWarnToken();
    }

    return vardecl;
}

ASTNode * parseProc(){
    ASTNode * proc = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    skipToken();
    if(!checkToken(TOKEN_KEYWORD, "procedure")){
        fprintf(fo, "not a procedure\n");
        proc->tok = wrapParserErrorToken();
        return proc;
    }
    proc->childs[0] = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    proc->childs[0]->tok = wrapTerminalToken("procedure");
    movePeeker();
    skipToken();
    if(!checkToken(TOKEN_ID, NULL)){
        fprintf(fo, "need ID in procedure\n");
        proc->tok = wrapParserErrorToken();
        return proc;
    }
    proc->childs[1] = (ASTNode*)arenaMalloc(sizeof(ASTNode)); 
    proc->childs[1] = parseId();
    skipToken();
    if(checkToken(TOKEN_SYMBOL, "(")){
        movePeeker();
        skipToken();
        ASTNode * procIDtracker = proc->childs[1]->childs[1]; //downgrade the arguments of a procedure into a linklist (another stupid solution :( )
        while(1){
            procIDtracker = (ASTNode*)arenaMalloc(sizeof(ASTNode));
            procIDtracker = parseId();
            skipToken();
            if(!checkToken(TOKEN_SYMBOL, ",")){
                break;
            }
            movePeeker();
            skipToken();
            procIDtracker = procIDtracker->childs[1];
        }
        if(!checkToken(TOKEN_SYMBOL, ")")){
            fprintf(fo, "need rigth \")\" after a left one in procudre arguments\n");
            proc->tok = wrapParserErrorToken();
            return proc;
        }
        movePeeker();
        skipToken();
    }
    if(!checkToken(TOKEN_SYMBOL, ";")){
        fprintf(fo, "need \";\" after a procedure declaration\n");
        proc->tok = wrapParserWarnToken();
    }
    movePeeker();
    skipToken();
    proc->childs[2] = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    proc->childs[2] = parseBody();
    skipToken();
    ASTNode * procProctracker = proc->childs[3];
    while(1){
        if(!checkToken(TOKEN_SYMBOL, ";")){
            break;
        }
        movePeeker();
        skipToken();
        procProctracker = (ASTNode*)arenaMalloc(sizeof(ASTNode));
        procProctracker->childs[3] = parseProc();
        skipToken();
    }   
    

    return proc;
}

ASTNode * parseBody(){
    ASTNode * body = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    skipToken();
    fprintf(fo, "debug here");
    return NULL; //TODO: debug here , implement printAST fisrt!!!!

    return body;
}

ASTNode * parseId(){
    skipToken();
    ASTNode * Id = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    if(!checkToken(TOKEN_ID, NULL)){
        fprintf(fo, "not a ID\n");
        Id->tok = wrapParserErrorToken();
        return Id;   
    }
    Id->tok = peeker[peekercnt];
    Id->tok.type = TERMINAL;
    movePeeker();

    return Id;
}

ASTNode * parseInteger(){
    skipToken();
    ASTNode * Integer = (ASTNode*)arenaMalloc(sizeof(ASTNode));
    if(!checkToken(TOKEN_DIGITAL, NULL)){
        fprintf(fo, "not a Integer\n");
        Integer->tok = wrapParserErrorToken();
        return Integer;
    }
    Integer->tok = peeker[peekercnt];
    Integer->tok.type = TERMINAL;

    return Integer; 
}

void printASTtree(ASTNode * node){
    if(node->tok.type == TERMINAL || node->tok.type == TOKEN_PARSER_ERROR || node->tok.type == TOKEN_PARSER_WARN){
        return;
    }
    fprintf(fo, node->tok.data);
    for(int i = 0; i < 4; i++){
        printASTtree(node->childs[i]);
    }
}
