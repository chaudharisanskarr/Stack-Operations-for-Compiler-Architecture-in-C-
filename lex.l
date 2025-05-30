%{
#include "parser.tab.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *token_type;
    char *lexeme;
    int line_number;
} Token;

#define INITIAL_CAPACITY 100

// Initialize token list with initial capacity
Token *token_list = NULL;
int token_count = 0;
int token_capacity = 0;
int line_number = 1;

// Initialize token list
void init_token_list() {
    if (token_list == NULL) {
        token_capacity = INITIAL_CAPACITY;
        token_list = (Token *)calloc(token_capacity, sizeof(Token));
        if (!token_list) {
            fprintf(stderr, "Memory allocation failed for token list\n");
            exit(1);
        }
    }
}

// Expand token list capacity when needed
void expand_token_list() {
    int new_capacity = token_capacity * 2;
    Token *new_list = (Token *)calloc(new_capacity, sizeof(Token));
    if (!new_list) {
        fprintf(stderr, "Memory allocation failed when expanding token list\n");
        exit(1);
    }
    
    // Copy existing tokens
    for (int i = 0; i < token_count; i++) {
        new_list[i].token_type = token_list[i].token_type;
        new_list[i].lexeme = token_list[i].lexeme;
        new_list[i].line_number = token_list[i].line_number;
    }
    
    // Free old array but not the strings inside (we're transferring ownership)
    free(token_list);
    
    token_list = new_list;
    token_capacity = new_capacity;
}

// Check if token already exists in the list
int isPresent(const char *text) {
    // Make sure the token list is initialized
    if (token_list == NULL) {
        init_token_list();
    }
    
    for (int i = 0; i < token_count; i++) {
        if (strcmp(text, token_list[i].lexeme) == 0) {
            return 0;
        }
    }
    return 1;
}

// Store a new token
void store_token(const char *type, const char *text) {
    // Make sure the token list is initialized
    if (token_list == NULL) {
        init_token_list();
    }
    
    if (isPresent(text)) {
        // Ensure we have space for a new token
        if (token_count >= token_capacity) {
            expand_token_list();
        }
        
        token_list[token_count].token_type = strdup(type);
        token_list[token_count].lexeme = strdup(text);
        token_list[token_count].line_number = line_number;
        token_count++;
    }
}

// Free all allocated memory
void cleanup_tokens() {
    if (token_list) {
        for (int i = 0; i < token_count; i++) {
            if (token_list[i].token_type) free(token_list[i].token_type);
            if (token_list[i].lexeme) free(token_list[i].lexeme);
        }
        free(token_list);
        token_list = NULL;
        token_count = 0;
        token_capacity = 0;
    }
}

%}

/* Define exclusive start conditions for comments */
%x C_COMMENT
%x CPP_COMMENT

%% 
"#include"            {store_token("Keyword", yytext); return PREPROCESSOR; }
[<>]                  {store_token("Punctuation", yytext); return SPECIAL_SYMBOL;}
"iostream"            {store_token("Identifiers", yytext); return HEADER_FILE; }
"stack"               {store_token("Identifiers", yytext); return STACK; }
"int"                 {store_token("DataTypes", yytext); return INT; }
"char*"               {store_token("DataTypes", yytext); return CHARSTAR; }
"void"                {store_token("Keyword", yytext); return VOID; }
"main"                {store_token("Identifiers", yytext); return MAIN;}
"using"               {store_token("Keyword", yytext); return USING; }
"namespace"           {store_token("Keyword", yytext); return NAMESPACE; }
"std"                 {store_token("Identifiers", yytext); return STD; }
"cout"                {store_token("STL_Function", yytext); return COUT; }
"endl"                {store_token("Keyword", yytext); return ENDL; }
"for"                 {store_token("Keyword", yytext); return FOR; }
"if"                  {store_token("Keyword", yytext); return IF; }
"return"              {store_token("Keyword", yytext); return RETURN; }
"size"                {store_token("Identifiers", yytext); return SIZE; }
"pop"                 {store_token("Identifiers", yytext); return POP; }
"push"                {store_token("Identifiers", yytext); return PUSH; }
"empty"               {store_token("Identifiers", yytext); return EMPTY; }
"top"                 {store_token("Identifiers", yytext); return TOP; }
"while"               {store_token("Keyword", yytext); return WHILE; }
"!="                  {store_token("Operators", yytext); return NOT_EQUAL; }

"/*"                  { BEGIN(C_COMMENT); /* Begin C-style comment */ }
<C_COMMENT>"*/"       { BEGIN(INITIAL); /* End C-style comment */ }
<C_COMMENT>\n         { line_number++; }
<C_COMMENT>.          { /* Ignore other characters in comment */ }

"//"                  { BEGIN(CPP_COMMENT); /* Begin C++ style comment */ }
<CPP_COMMENT>\n       { line_number++; BEGIN(INITIAL); /* End C++ comment */ }
<CPP_COMMENT>.        { /* Ignore other characters in comment */ }

"\""([^\"\n]+)"\""    {yylval.str = strdup(yytext); store_token("Literals", yytext); return STRING_LITERAL; }
"("                   {store_token("Punctuation", yytext); return LPAREN; }
")"                   {store_token("Punctuation", yytext); return RPAREN; }
"{"                   {store_token("Punctuation", yytext); return LBRACE; }
"}"                   {store_token("Punctuation", yytext); return RBRACE; }
";"                   {store_token("Punctuation", yytext); return TERMINATOR_SYMBOL; }
","                   {store_token("Punctuation", yytext); return COMMA; }
"="                   {store_token("Operators", yytext); return ASSIGNMENT_OPERATOR; }
"+"                   {store_token("Operators", yytext); return INCREMENT_OPERATOR;}
"<<"                  {store_token("Operators", yytext); return OPERATOR; }
"["                   {store_token("Punctuation", yytext); return LBRACKET; }
"]"                   {store_token("Punctuation", yytext); return RBRACKET; }
"."                   {store_token("Operators", yytext); return DOT;}
[0-9]+                {yylval.num = atoi(yytext); store_token("Literals", yytext); return NUMBER; }
[A-Za-z_][A-Za-z0-9_]* {yylval.id = strdup(yytext); store_token("Identifiers", yytext); return IDENTIFIER; }
[\n]                  {line_number++;}
[ \t\r]+              { /* ignore whitespace */ }
.                     { /* ignore unrecognized characters */ }
%%

int yywrap() {
    return 1;
}

void print_tokens() {
    if (token_list == NULL) {
        printf("No tokens found.\n");
        return;
    }

    printf("Total Tokens : %d\n", token_count);
    printf("=======================================================\n");
    printf("| %-20s | %-15s | %-10s |\n", "Token Type", "Lexeme", "Line No.");
    printf("=======================================================\n");

    // Identifiers
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "Identifiers") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("_______________________________________________________\n");
    // Operators
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "Operators") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("_______________________________________________________\n");
    // Punctuation
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "Punctuation") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("_______________________________________________________\n");
    // Keywords
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "Keyword") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("_______________________________________________________\n");
    // Literals
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "Literals") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("_______________________________________________________\n");
    // Standard Library function
    for (int i = 0; i < token_count; i++) {
        if (token_list[i].token_type && strcmp(token_list[i].token_type, "STL_Function") == 0) {
            printf("| %-20s | %-15s | %-10d |\n", token_list[i].token_type, token_list[i].lexeme, token_list[i].line_number);
        }
    }
    printf("=======================================================\n\n");
}