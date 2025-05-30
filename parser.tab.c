/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

int yylex(void);
int yyerror(const char *s);
extern FILE *yyin;
void print_tokens();

char* current_function_name = NULL;

/* AST Node Types */
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_FUNCTION_CALL,
    AST_VARIABLE_DECL,
    AST_STACK_DECL,
    AST_STACK_OP,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_RETURN,
    AST_COUT,
    AST_PREPROCESSOR,
    AST_STD_DECL
} AstNodeType;

/* AST Node Structure */
typedef struct AstNode {
    AstNodeType type;
    char* value;
    int num_children;
    struct AstNode** children;
} AstNode;

/* Create a new AST node */
AstNode* create_ast_node(AstNodeType type, const char* value) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for AST node\n");
        return NULL;
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->num_children = 0;
    node->children = NULL;
    return node;
}

/* Add a child to an AST node */
void add_ast_child(AstNode* parent, AstNode* child) {
    if (parent == NULL || child == NULL) return;
    
    parent->num_children++;
    parent->children = (AstNode**)realloc(parent->children, parent->num_children * sizeof(AstNode*));
    if (!parent->children) {
        fprintf(stderr, "Memory allocation failed for AST children\n");
        return;
    }
    parent->children[parent->num_children - 1] = child;
}

/* Print the AST with indentation */
void print_ast(AstNode* node, int indent) {
    if (node == NULL) return;
    
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    const char* type_str = "";
    switch (node->type) {
        case AST_PROGRAM: type_str = "PROGRAM"; break;
        case AST_FUNCTION: type_str = "FUNCTION"; break;
        case AST_FUNCTION_CALL: type_str = "FUNCTION_CALL"; break;
        case AST_VARIABLE_DECL: type_str = "VARIABLE_DECL"; break;
        case AST_STACK_DECL: type_str = "STACK_DECL"; break;
        case AST_STACK_OP: type_str = "STACK_OP"; break;
        case AST_WHILE_LOOP: type_str = "WHILE_LOOP"; break;
        case AST_FOR_LOOP: type_str = "FOR_LOOP"; break;
        case AST_RETURN: type_str = "RETURN"; break;
        case AST_COUT: type_str = "COUT"; break;
        case AST_PREPROCESSOR: type_str = "PREPROCESSOR"; break;
        case AST_STD_DECL: type_str = "STD_DECL"; break;
    }
    
    printf("%s", type_str);
    if (node->value) {
        printf(" [%s]", node->value);
    }
    printf("\n");
    
    for (int i = 0; i < node->num_children; i++) {
        print_ast(node->children[i], indent + 1);
    }
}

/* Free AST memory */
void free_ast(AstNode* node) {
    if (node == NULL) return;
    
    for (int i = 0; i < node->num_children; i++) {
        free_ast(node->children[i]);
    }
    
    if (node->children) free(node->children);
    if (node->value) free(node->value);
    free(node);
}

/* Root of the AST */
AstNode* ast_root = NULL;

typedef enum { INT_TYPE, VOID_TYPE, CHAR_TYPE, STRING_TYPE, STACK_INT_TYPE, NONE_TYPE } VarType;
typedef enum { NONE, STATIC, STACK_SIZE, STACK_TOP } valueType;

typedef struct {
    char* variable;
    VarType type; 
    char* scope;
    bool isFunction;
    int param_count;
    VarType argument_type;
    valueType val_type;
} Symbol;

VarType argument_type;

Symbol symbol_table[100];
int symbol_count = 0;

typedef struct {
    char* variable;
    char* data_type;
    char* scope;
    bool isActual;
    int size;
} stackInfo;

stackInfo stackData[100];
int stack_count = 0;

// Helper function to convert string number to data type
char* get_data_type_from_value(const char* value) {
    // If the value is a number (starts with a digit), return "INT"
    if (value && value[0] >= '0' && value[0] <= '9') {
        return "INT";
    }
    // Otherwise, return "STRING"
    return "STRING";
}

void stack_push_ele(const char* variable, const char* data_type, const char* scope) {
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
           stackData[i].size++;
           return;
        }
    }
    printf("\n!!!!!Stack '%s' not found in scope '%s'", variable, scope);
    // Don't exit - just report
}

void stack_top_ele(const char* variable, const char* scope) {
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
            if(stackData[i].size == 0) {
                printf("\n!!!!!Stack Underflow Warning");
                // Don't exit here - just warn
                return;
            }
            return;
        }
    }
    printf("\n!!!!!Stack '%s' not found in scope '%s'", variable, scope);
    // Don't exit
}

void stack_typemismatch1(const char* function, const char* scope, const char* variable) {
    char* data_type_actual = NULL;
    char* data_type_virtual = NULL;
    bool flag1 = false;
    bool flag2 = false;
    
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
           data_type_actual = stackData[i].data_type;
           flag1 = true;
           break;
        }
    }
   
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].scope, function) == 0) {
           data_type_virtual = stackData[i].data_type;
           flag2 = true;
           break;
        }
    }
    
    if(flag1 == false && flag2 == true) {
        printf("\n!!!!Stack Datatype Mismatch Warning");
        // Don't exit
    }
    
    if(flag1 == true && flag2 == true) {
        if(strcmp(data_type_actual, data_type_virtual) != 0) {
            printf("\n!!!!Stack Datatype Mismatch Warning");
            // Don't exit
        }
    }
}

void stack_typemismatch(const char* variable, const char* scope, const char* value) {
    char* data_type_actual = NULL;
    bool found = false;
    
    // Get expected data type from the stack declaration
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
           data_type_actual = stackData[i].data_type;
           found = true;
           break;
        }
    }
    
    if (!found) {
        printf("\n!!!!!Stack '%s' not found in scope '%s'", variable, scope);
        // Don't exit
        return;
    }
   
    // Convert value to data type and compare
    char* data_type = get_data_type_from_value(value);
    
    // For numbers, we handle them as INTs regardless of representation
    if (strcmp(data_type, "INT") == 0 && strcmp(data_type_actual, "INT") == 0) {
        // Both are INTs, no warning needed
        return;
    }
    
    // Otherwise check for mismatch
    if(strcmp(data_type_actual, data_type) != 0) {
        printf("\n!!!!Stack Datatype Mismatch Warning: Expected %s, got %s", data_type_actual, value);
        // Don't exit
    }
}

void stack_pop_ele(const char* variable, const char* scope) {
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
            if(stackData[i].size == 0) {
                printf("\n!!!!!Stack Underflow Warning");
                // Don't exit - just warn
                return;
            } else {
                stackData[i].size--;
            }
            return;
        }
    }
    printf("\n!!!!!Stack '%s' not found in scope '%s'", variable, scope);
    // Don't exit
}

void add_stack(const char* variable, const char* data_type, const char* scope, bool isActual) {
    // Check if stack already exists in the same scope
    for (int i = 0; i < stack_count; i++) {
        if (strcmp(stackData[i].variable, variable) == 0 && strcmp(stackData[i].scope, scope) == 0) {
            printf("\n!!!!!Stack '%s' already declared in scope '%s'", variable, scope);
            // Don't exit
            return;
        }
    }
    
    stackData[stack_count].variable = strdup(variable);
    stackData[stack_count].data_type = strdup(data_type);
    stackData[stack_count].scope = strdup(scope);
    stackData[stack_count].isActual = isActual;
    stackData[stack_count].size = 0;
    stack_count++;
}

int isVariableDeclared(const char* var, const char* scope) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].variable, var) == 0 && strcmp(symbol_table[i].scope, scope) == 0) {
            return i;
        }
    }
    return -1;
}

int isFunctionDeclared(const char* var) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].variable, var) == 0 && symbol_table[i].isFunction) {
            return i;
        }
    }
    return -1;
}

void add_symbol(const char* var, VarType type, const char* scope, bool isFunction) {
    if (!var || !scope) {
        printf("\nError: Null variable or scope name\n");
        return; // Don't exit
    }
    
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].variable, var) == 0 && strcmp(symbol_table[i].scope, scope) == 0) {
            printf("\nSemantic Warning - Redeclaration of variable '%s' in scope '%s'!!!!\n\n", var, scope);
            return; // Don't exit
        }
    }
    
    symbol_table[symbol_count].variable = strdup(var);
    symbol_table[symbol_count].type = type;
    symbol_table[symbol_count].scope = strdup(scope);
    symbol_table[symbol_count].isFunction = isFunction;
    symbol_table[symbol_count].param_count = 0;
    symbol_table[symbol_count].val_type = NONE;
    symbol_table[symbol_count].argument_type = NONE_TYPE;
    symbol_count++;
}

void show_symbol_table() {
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-15s | %-10s | %-15s | %-10s | %-15s  | %-10s | %-10s  |\n", "Variable", "Type", "Scope", "Is Func", "Total Argument", "Value Type", "Argu Type");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < symbol_count; i++) {
        printf("| %-15s | ", symbol_table[i].variable);
        if (symbol_table[i].type == INT_TYPE) {
            printf("%-10s | ", "Integer");
        } else if (symbol_table[i].type == CHAR_TYPE) {
            printf("%-10s | ", "Character");
        } else if (symbol_table[i].type == STRING_TYPE) {
            printf("%-10s | ", "String");
        } else if (symbol_table[i].type == VOID_TYPE) {
            printf("%-10s | ", "Void");
        } else if (symbol_table[i].type == STACK_INT_TYPE) {
            printf("%-10s | ", "Stack");
        }
        printf("%-15s | %-10s | ", symbol_table[i].scope, symbol_table[i].isFunction ? "Yes" : "No");
        if(symbol_table[i].isFunction) {
            printf(" %-15d | ", symbol_table[i].param_count);
        } else {
            printf(" %-15s | ", "-");
        }
        if (symbol_table[i].val_type == STATIC) {
            printf("%-10s | ", "Static");
        } else if (symbol_table[i].val_type == NONE) {
            printf("%-10s | ", "None");
        } else if (symbol_table[i].val_type == STACK_SIZE) {
            printf("%-10s | ", "Stack Size");
        } else if (symbol_table[i].val_type == STACK_TOP) {
            printf("%-10s | ", "Stack Top");
        }
        
        if(symbol_table[i].isFunction) {
            if (symbol_table[i].argument_type == INT_TYPE) {
                printf("%-10s | ", "Integer");
            } else if (symbol_table[i].argument_type == CHAR_TYPE) {
                printf("%-10s | ", "Character");
            } else if (symbol_table[i].argument_type == STRING_TYPE) {
                printf("%-10s | ", "String");
            } else if (symbol_table[i].argument_type == VOID_TYPE) {
                printf("%-10s | ", "Void");
            } else if (symbol_table[i].argument_type == STACK_INT_TYPE) {
                printf("%-10s | ", "Stack");
            } else if (symbol_table[i].argument_type == NONE_TYPE) {
                printf("%-10s | ", "none");
            }
        } else {
            printf(" %-10s | ", "-");
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------------------------------------------------\n\n");
}


#line 455 "parser.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_PREPROCESSOR = 3,               /* PREPROCESSOR  */
  YYSYMBOL_HEADER_FILE = 4,                /* HEADER_FILE  */
  YYSYMBOL_SPECIAL_SYMBOL = 5,             /* SPECIAL_SYMBOL  */
  YYSYMBOL_USING = 6,                      /* USING  */
  YYSYMBOL_NAMESPACE = 7,                  /* NAMESPACE  */
  YYSYMBOL_STD = 8,                        /* STD  */
  YYSYMBOL_INT = 9,                        /* INT  */
  YYSYMBOL_VOID = 10,                      /* VOID  */
  YYSYMBOL_MAIN = 11,                      /* MAIN  */
  YYSYMBOL_CHARSTAR = 12,                  /* CHARSTAR  */
  YYSYMBOL_IF = 13,                        /* IF  */
  YYSYMBOL_ELSE = 14,                      /* ELSE  */
  YYSYMBOL_FOR = 15,                       /* FOR  */
  YYSYMBOL_ASSIGNMENT_OPERATOR = 16,       /* ASSIGNMENT_OPERATOR  */
  YYSYMBOL_INCREMENT_OPERATOR = 17,        /* INCREMENT_OPERATOR  */
  YYSYMBOL_RETURN = 18,                    /* RETURN  */
  YYSYMBOL_COUT = 19,                      /* COUT  */
  YYSYMBOL_ENDL = 20,                      /* ENDL  */
  YYSYMBOL_OPERATOR = 21,                  /* OPERATOR  */
  YYSYMBOL_STACK = 22,                     /* STACK  */
  YYSYMBOL_SIZE = 23,                      /* SIZE  */
  YYSYMBOL_POP = 24,                       /* POP  */
  YYSYMBOL_PUSH = 25,                      /* PUSH  */
  YYSYMBOL_EMPTY = 26,                     /* EMPTY  */
  YYSYMBOL_TOP = 27,                       /* TOP  */
  YYSYMBOL_DOT = 28,                       /* DOT  */
  YYSYMBOL_LPAREN = 29,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 30,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 31,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 32,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 33,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 34,                  /* RBRACKET  */
  YYSYMBOL_TERMINATOR_SYMBOL = 35,         /* TERMINATOR_SYMBOL  */
  YYSYMBOL_COMMA = 36,                     /* COMMA  */
  YYSYMBOL_WHILE = 37,                     /* WHILE  */
  YYSYMBOL_NOT_EQUAL = 38,                 /* NOT_EQUAL  */
  YYSYMBOL_IDENTIFIER = 39,                /* IDENTIFIER  */
  YYSYMBOL_STRING_LITERAL = 40,            /* STRING_LITERAL  */
  YYSYMBOL_NUMBER = 41,                    /* NUMBER  */
  YYSYMBOL_YYACCEPT = 42,                  /* $accept  */
  YYSYMBOL_program = 43,                   /* program  */
  YYSYMBOL_preprocessor_statement_list = 44, /* preprocessor_statement_list  */
  YYSYMBOL_preprocessor_statement = 45,    /* preprocessor_statement  */
  YYSYMBOL_std_declaration = 46,           /* std_declaration  */
  YYSYMBOL_function_list = 47,             /* function_list  */
  YYSYMBOL_function = 48,                  /* function  */
  YYSYMBOL_function_signature = 49,        /* function_signature  */
  YYSYMBOL_function_declaration_name = 50, /* function_declaration_name  */
  YYSYMBOL_variable_declaration_name = 51, /* variable_declaration_name  */
  YYSYMBOL_function_parameter_list = 52,   /* function_parameter_list  */
  YYSYMBOL_function_parameter = 53,        /* function_parameter  */
  YYSYMBOL_function_definition = 54,       /* function_definition  */
  YYSYMBOL_function_call = 55,             /* function_call  */
  YYSYMBOL_function_name = 56,             /* function_name  */
  YYSYMBOL_argument_list = 57,             /* argument_list  */
  YYSYMBOL_argument = 58,                  /* argument  */
  YYSYMBOL_statement_list = 59,            /* statement_list  */
  YYSYMBOL_statement = 60,                 /* statement  */
  YYSYMBOL_cout_statement = 61,            /* cout_statement  */
  YYSYMBOL_variable_declaration = 62,      /* variable_declaration  */
  YYSYMBOL_variable_value = 63,            /* variable_value  */
  YYSYMBOL_for_loop_signature = 64,        /* for_loop_signature  */
  YYSYMBOL_while_loop = 65,                /* while_loop  */
  YYSYMBOL_while_condition = 66,           /* while_condition  */
  YYSYMBOL_initialization_block = 67,      /* initialization_block  */
  YYSYMBOL_condition_block = 68,           /* condition_block  */
  YYSYMBOL_action_block = 69,              /* action_block  */
  YYSYMBOL_for_loop = 70,                  /* for_loop  */
  YYSYMBOL_stack_declaration_stmt = 71,    /* stack_declaration_stmt  */
  YYSYMBOL_stack_declaration = 72,         /* stack_declaration  */
  YYSYMBOL_stack_push = 73,                /* stack_push  */
  YYSYMBOL_stack_size = 74,                /* stack_size  */
  YYSYMBOL_stack_pop = 75,                 /* stack_pop  */
  YYSYMBOL_stack_top = 76,                 /* stack_top  */
  YYSYMBOL_main_function = 77,             /* main_function  */
  YYSYMBOL_main_function_signature = 78,   /* main_function_signature  */
  YYSYMBOL_main_function_definition = 79,  /* main_function_definition  */
  YYSYMBOL_return_statement = 80           /* return_statement  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   230

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  42
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  39
/* YYNRULES -- Number of rules.  */
#define YYNRULES  74
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  183

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   296


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   426,   426,   440,   446,   454,   459,   466,   473,   478,
     484,   490,   501,   508,   517,   525,   535,   541,   549,   553,
     559,   570,   581,   588,   608,   624,   631,   635,   642,   655,
     660,   666,   672,   676,   680,   684,   688,   692,   696,   700,
     704,   708,   715,   720,   728,   733,   741,   751,   756,   769,
     781,   793,   801,   809,   816,   823,   832,   838,   850,   863,
     869,   878,   884,   895,   902,   911,   923,   938,   950,   964,
     978,   990,  1002,  1009,  1015
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "PREPROCESSOR",
  "HEADER_FILE", "SPECIAL_SYMBOL", "USING", "NAMESPACE", "STD", "INT",
  "VOID", "MAIN", "CHARSTAR", "IF", "ELSE", "FOR", "ASSIGNMENT_OPERATOR",
  "INCREMENT_OPERATOR", "RETURN", "COUT", "ENDL", "OPERATOR", "STACK",
  "SIZE", "POP", "PUSH", "EMPTY", "TOP", "DOT", "LPAREN", "RPAREN",
  "LBRACE", "RBRACE", "LBRACKET", "RBRACKET", "TERMINATOR_SYMBOL", "COMMA",
  "WHILE", "NOT_EQUAL", "IDENTIFIER", "STRING_LITERAL", "NUMBER",
  "$accept", "program", "preprocessor_statement_list",
  "preprocessor_statement", "std_declaration", "function_list", "function",
  "function_signature", "function_declaration_name",
  "variable_declaration_name", "function_parameter_list",
  "function_parameter", "function_definition", "function_call",
  "function_name", "argument_list", "argument", "statement_list",
  "statement", "cout_statement", "variable_declaration", "variable_value",
  "for_loop_signature", "while_loop", "while_condition",
  "initialization_block", "condition_block", "action_block", "for_loop",
  "stack_declaration_stmt", "stack_declaration", "stack_push",
  "stack_size", "stack_pop", "stack_top", "main_function",
  "main_function_signature", "main_function_definition",
  "return_statement", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-47)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      23,    41,    58,    80,   -47,    16,   -47,    63,   -47,    32,
      91,    92,    90,    60,    61,    53,   -47,    71,    72,   -47,
     -47,    69,   -47,   -47,    -4,   -47,   -47,    75,    66,   -47,
       9,   -47,    78,    66,   -47,    73,    74,    79,    10,    88,
     105,    82,    86,   -11,   -47,    93,    -7,   -47,   -47,   -47,
      84,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,    81,
     112,   -47,    31,   -47,    89,    18,   -47,   -47,    -5,   -47,
      94,    51,     4,    54,    70,    38,   -47,   -16,   -47,   -47,
      66,   -47,   114,   -47,     0,   -47,   -47,    85,   109,    95,
     -47,    26,   100,   121,   122,    96,     1,    98,   -47,   102,
     103,   106,   -47,   -47,   101,   -47,   104,   -47,    44,   -47,
      34,   128,   -47,   124,    97,   107,   117,   115,   123,   108,
     110,   113,   111,   116,   118,   120,   126,    38,   125,   -47,
     -47,   119,   127,   -47,   129,   130,   -47,     5,   132,   134,
     131,   135,   137,   138,   133,   -47,   -47,   136,    66,   139,
     145,   147,   -47,   -47,   -47,   -47,   140,   -47,   148,   -47,
     -47,   -47,   151,   152,    50,   -47,   142,   153,   141,   154,
     143,   155,   -47,   -47,   -47,   -47,   144,   -47,   161,   -47,
     -47,   156,   -47
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     0,     3,     0,     1,     0,     4,     0,
       0,     0,     0,     0,     0,     0,     8,     0,     0,     5,
       6,     0,    15,    14,     0,     9,     2,     0,    31,    11,
       0,     7,     0,    31,    70,     0,     0,     0,     0,     0,
       0,     0,    25,     0,    38,     0,     0,    29,    41,    40,
       0,    33,    32,    36,    63,    37,    35,    34,    39,     0,
       0,    12,     0,    18,     0,     0,    17,    16,    56,    74,
       0,     0,     0,    59,     0,     0,    44,     0,    22,    30,
      31,    21,     0,    13,     0,    71,    72,     0,     0,     0,
      73,     0,     0,     0,     0,     0,     0,     0,    53,     0,
       0,     0,    47,    46,     0,    48,     0,    28,     0,    26,
       0,     0,    19,     0,     0,    59,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
      24,     0,     0,    62,     0,     0,    55,     0,     0,     0,
       0,     0,     0,     0,     0,    57,    58,     0,    31,     0,
       0,     0,    23,    27,    20,    54,    61,    43,     0,    42,
      64,    65,     0,     0,     0,    68,     0,     0,     0,     0,
       0,     0,    52,    50,    66,    67,     0,    49,     0,    51,
      60,     0,    69
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -47,   -47,   -47,   149,   -47,   -47,   171,   -47,   -47,   -47,
     -47,   146,   -47,   -47,   -47,   -47,    13,   -32,   -46,   -47,
     -47,    30,   -47,   -47,   -47,   -47,    47,   -47,   -47,   -47,
     -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47,   -47
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,     3,     4,     9,    15,    16,    17,    18,    43,
      62,    63,    29,    44,    45,   108,   109,    46,    47,    48,
      49,   104,    50,    51,    97,    89,    98,   169,    52,    53,
      54,    55,   105,    56,    57,    26,    27,    34,    58
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      79,    65,    35,    36,    87,    75,   122,    32,    37,    59,
     122,    38,    39,    93,   106,    40,    94,   123,    59,    79,
      10,   123,    60,   107,    76,    78,     1,    35,    36,   124,
      41,    60,    42,    37,    88,    22,    38,    39,    11,    61,
      40,    13,    14,    35,    36,    69,     5,   116,   110,    37,
      86,    70,    38,    39,   117,    41,    40,    42,     6,    35,
      36,    83,    24,    14,    79,    37,   133,    84,    38,    39,
      12,    41,    40,    42,   131,    35,    36,   101,   102,   103,
     132,    37,   173,     1,    38,    39,     7,    41,    40,    42,
      91,    92,    95,    96,    99,   100,    19,    20,    21,    22,
      23,    30,    28,    41,    31,    42,    33,    64,    68,    71,
      72,    73,    66,    67,    74,    80,   164,    82,    79,    85,
      81,   118,    77,   111,   113,   114,   119,   120,   125,    90,
     115,   126,   127,   134,   128,   121,   129,   139,   136,   130,
     135,   144,   140,   141,   147,   153,   137,   142,   151,   143,
     145,   148,     8,     0,   152,   146,   149,   150,   176,   162,
     158,   180,   138,     0,   178,   163,   107,   156,   154,   157,
     159,   155,   160,   161,   165,   166,   167,   174,   170,   168,
     171,   181,   172,   175,   177,   179,    25,     0,     0,     0,
       0,   182,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     112
};

static const yytype_int16 yycheck[] =
{
      46,    33,     9,    10,     9,    16,     5,    11,    15,     9,
       5,    18,    19,     9,    30,    22,    12,    16,     9,    65,
       4,    16,    22,    39,    35,    32,     3,     9,    10,    28,
      37,    22,    39,    15,    39,    39,    18,    19,    22,    30,
      22,     9,    10,     9,    10,    35,     5,    21,    80,    15,
      32,    41,    18,    19,    28,    37,    22,    39,     0,     9,
      10,    30,     9,    10,   110,    15,    32,    36,    18,    19,
       7,    37,    22,    39,    30,     9,    10,    39,    40,    41,
      36,    15,    32,     3,    18,    19,     6,    37,    22,    39,
      39,    40,    38,    39,    24,    25,     5,     5,     8,    39,
      39,    29,    31,    37,    35,    39,    31,    29,    29,    21,
       5,    29,    39,    39,    28,    31,   148,     5,   164,    30,
      39,    21,    29,     9,    39,    16,     5,     5,    30,    35,
      35,    29,    29,     5,    28,    39,    35,    20,    41,    35,
      16,    28,    27,    20,    26,   132,    39,    39,    23,    39,
      39,    31,     3,    -1,    35,    39,    30,   127,    17,    26,
      29,    17,   115,    -1,    21,    29,    39,    35,    39,    35,
      35,    41,    35,    35,    35,    30,    29,    35,    30,    39,
      29,    20,    30,    30,    30,    30,    15,    -1,    -1,    -1,
      -1,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    43,    44,    45,     5,     0,     6,    45,    46,
       4,    22,     7,     9,    10,    47,    48,    49,    50,     5,
       5,     8,    39,    39,     9,    48,    77,    78,    31,    54,
      29,    35,    11,    31,    79,     9,    10,    15,    18,    19,
      22,    37,    39,    51,    55,    56,    59,    60,    61,    62,
      64,    65,    70,    71,    72,    73,    75,    76,    80,     9,
      22,    30,    52,    53,    29,    59,    39,    39,    29,    35,
      41,    21,     5,    29,    28,    16,    35,    29,    32,    60,
      31,    39,     5,    30,    36,    30,    32,     9,    39,    67,
      35,    39,    40,     9,    12,    38,    39,    66,    68,    24,
      25,    39,    40,    41,    63,    74,    30,    39,    57,    58,
      59,     9,    53,    39,    16,    35,    21,    28,    21,     5,
       5,    39,     5,    16,    28,    30,    29,    29,    28,    35,
      35,    30,    36,    32,     5,    16,    41,    39,    68,    20,
      27,    20,    39,    39,    28,    39,    39,    26,    31,    30,
      63,    23,    35,    58,    39,    41,    35,    35,    29,    35,
      35,    35,    26,    29,    59,    35,    30,    29,    39,    69,
      30,    29,    30,    32,    35,    30,    17,    30,    21,    30,
      17,    20,    35
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    42,    43,    44,    44,    45,    45,    46,    47,    47,
      47,    48,    49,    49,    50,    50,    51,    51,    52,    52,
      53,    53,    54,    55,    55,    56,    57,    57,    58,    59,
      59,    59,    60,    60,    60,    60,    60,    60,    60,    60,
      60,    60,    61,    61,    62,    62,    63,    63,    63,    64,
      65,    66,    66,    66,    67,    67,    67,    68,    68,    68,
      69,    69,    70,    71,    72,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    80
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     4,     1,     2,     4,     4,     4,     1,     2,
       0,     2,     3,     4,     2,     2,     2,     2,     1,     3,
       5,     2,     3,     5,     4,     1,     1,     3,     1,     1,
       2,     0,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     6,     6,     2,     4,     1,     1,     1,     8,
       7,     6,     5,     1,     4,     3,     0,     3,     3,     0,
       3,     0,     4,     1,     6,     6,     7,     5,     6,    10,
       2,     4,     3,     3,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: preprocessor_statement_list std_declaration function_list main_function  */
#line 427 "parser.y"
    {
        printf("Program processed.\n");
        ast_root = create_ast_node(AST_PROGRAM, "program");
        if (ast_root) {
            add_ast_child(ast_root, (yyvsp[-3].ast));
            add_ast_child(ast_root, (yyvsp[-2].ast));
            add_ast_child(ast_root, (yyvsp[-1].ast));
            add_ast_child(ast_root, (yyvsp[0].ast));
        }
        (yyval.ast) = ast_root;
    }
#line 1667 "parser.tab.c"
    break;

  case 3: /* preprocessor_statement_list: preprocessor_statement  */
#line 441 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_PREPROCESSOR, "preprocessor_list");
        if ((yyval.ast)) add_ast_child((yyval.ast), (yyvsp[0].ast));
        printf("Preprocessor statement processed.\n");
    }
#line 1677 "parser.tab.c"
    break;

  case 4: /* preprocessor_statement_list: preprocessor_statement_list preprocessor_statement  */
#line 447 "parser.y"
    {
        if ((yyvsp[-1].ast) && (yyvsp[0].ast)) add_ast_child((yyvsp[-1].ast), (yyvsp[0].ast));
        (yyval.ast) = (yyvsp[-1].ast);
        printf("Preprocessor statement_list processed.\n");
    }
#line 1687 "parser.tab.c"
    break;

  case 5: /* preprocessor_statement: PREPROCESSOR SPECIAL_SYMBOL HEADER_FILE SPECIAL_SYMBOL  */
#line 455 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_PREPROCESSOR, "include");
        printf("Processed preprocessor statement.\n");
    }
#line 1696 "parser.tab.c"
    break;

  case 6: /* preprocessor_statement: PREPROCESSOR SPECIAL_SYMBOL STACK SPECIAL_SYMBOL  */
#line 460 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_PREPROCESSOR, "include_stack");
        printf("Processed preprocessor statement.\n");
    }
#line 1705 "parser.tab.c"
    break;

  case 7: /* std_declaration: USING NAMESPACE STD TERMINATOR_SYMBOL  */
#line 467 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STD_DECL, "std_declaration");
        printf("Processed std statement.\n");
    }
#line 1714 "parser.tab.c"
    break;

  case 8: /* function_list: function  */
#line 474 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "function_list");
        if ((yyval.ast)) add_ast_child((yyval.ast), (yyvsp[0].ast));
    }
#line 1723 "parser.tab.c"
    break;

  case 9: /* function_list: function_list function  */
#line 479 "parser.y"
    {
        if ((yyvsp[-1].ast) && (yyvsp[0].ast)) add_ast_child((yyvsp[-1].ast), (yyvsp[0].ast));
        (yyval.ast) = (yyvsp[-1].ast);
    }
#line 1732 "parser.tab.c"
    break;

  case 10: /* function_list: %empty  */
#line 484 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "empty_function_list");
        printf("function_list processed.\n");
    }
#line 1741 "parser.tab.c"
    break;

  case 11: /* function: function_signature function_definition  */
#line 491 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "function");
        if ((yyval.ast)) {
            add_ast_child((yyval.ast), (yyvsp[-1].ast));
            add_ast_child((yyval.ast), (yyvsp[0].ast));
        }
        printf("function processed.\n");
    }
#line 1754 "parser.tab.c"
    break;

  case 12: /* function_signature: function_declaration_name LPAREN RPAREN  */
#line 502 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "function_signature_no_params");
        if ((yyval.ast)) add_ast_child((yyval.ast), (yyvsp[-2].ast));
        symbol_table[symbol_count - 1].param_count = 0;
        printf("function signature.\n");
    }
#line 1765 "parser.tab.c"
    break;

  case 13: /* function_signature: function_declaration_name LPAREN function_parameter_list RPAREN  */
#line 509 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "function_signature_with_params");
        if ((yyval.ast)) add_ast_child((yyval.ast), (yyvsp[-3].ast));
        symbol_table[symbol_count - 1].param_count = (yyvsp[-1].num);
        printf("function signature with parameters.\n");
    }
#line 1776 "parser.tab.c"
    break;

  case 14: /* function_declaration_name: VOID IDENTIFIER  */
#line 518 "parser.y"
    {
        current_function_name = strdup((yyvsp[0].id));
        add_symbol((yyvsp[0].id), VOID_TYPE, current_function_name, true); 
        symbol_table[symbol_count - 1].val_type = NONE;
        (yyval.ast) = create_ast_node(AST_FUNCTION, (yyvsp[0].id));
        printf("void function_declaration_name: %s\n", (yyvsp[0].id));
    }
#line 1788 "parser.tab.c"
    break;

  case 15: /* function_declaration_name: INT IDENTIFIER  */
#line 526 "parser.y"
    {
        current_function_name = strdup((yyvsp[0].id));
        add_symbol((yyvsp[0].id), INT_TYPE, current_function_name, true); 
        symbol_table[symbol_count - 1].val_type = NONE;
        (yyval.ast) = create_ast_node(AST_FUNCTION, (yyvsp[0].id));
        printf("int function_declaration_name: %s\n", (yyvsp[0].id));
    }
#line 1800 "parser.tab.c"
    break;

  case 16: /* variable_declaration_name: VOID IDENTIFIER  */
#line 536 "parser.y"
    {
        add_symbol((yyvsp[0].id), VOID_TYPE, current_function_name, false); 
        symbol_table[symbol_count - 1].param_count = 0;
        (yyval.str) = (yyvsp[0].id);
    }
#line 1810 "parser.tab.c"
    break;

  case 17: /* variable_declaration_name: INT IDENTIFIER  */
#line 542 "parser.y"
    {
        add_symbol((yyvsp[0].id), INT_TYPE, current_function_name, false); 
        symbol_table[symbol_count - 1].param_count = 0;
        (yyval.str) = (yyvsp[0].id);
    }
#line 1820 "parser.tab.c"
    break;

  case 18: /* function_parameter_list: function_parameter  */
#line 550 "parser.y"
    {
        (yyval.num) = 1;
    }
#line 1828 "parser.tab.c"
    break;

  case 19: /* function_parameter_list: function_parameter_list COMMA function_parameter  */
#line 554 "parser.y"
    {
        (yyval.num) = (yyvsp[-2].num) + 1;
    }
#line 1836 "parser.tab.c"
    break;

  case 20: /* function_parameter: STACK SPECIAL_SYMBOL INT SPECIAL_SYMBOL IDENTIFIER  */
#line 560 "parser.y"
    {
        printf("function parameter stack<int>\n");
        add_symbol((yyvsp[0].id), STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
        symbol_table[symbol_count - 2].argument_type = STACK_INT_TYPE;

        add_stack((yyvsp[0].id), "INT", current_function_name, false);
        (yyval.ast) = create_ast_node(AST_STACK_DECL, (yyvsp[0].id));
    }
#line 1851 "parser.tab.c"
    break;

  case 21: /* function_parameter: INT IDENTIFIER  */
#line 571 "parser.y"
    {
        add_symbol((yyvsp[0].id), INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
        symbol_table[symbol_count - 2].argument_type = INT_TYPE;
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, (yyvsp[0].id));
    }
#line 1863 "parser.tab.c"
    break;

  case 22: /* function_definition: LBRACE statement_list RBRACE  */
#line 582 "parser.y"
    {
        (yyval.ast) = (yyvsp[-1].ast);
        printf("function definition processed\n");
    }
#line 1872 "parser.tab.c"
    break;

  case 23: /* function_call: function_name LPAREN argument_list RPAREN TERMINATOR_SYMBOL  */
#line 589 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION_CALL, (yyvsp[-4].str));
        int index = isFunctionDeclared((yyvsp[-4].str));
        if (index == -1) {
            printf("\nFunction %s is not declared!!!!\n", (yyvsp[-4].str));
            // Don't exit
        }
        else if(symbol_table[index].param_count != (yyvsp[-2].num)) {
            printf("\nFunction %s Argument Mismatch error!!!! Expected %d arguments, got %d.\n\n", 
                   (yyvsp[-4].str), symbol_table[index].param_count, (yyvsp[-2].num));
            // Don't exit
        }
        else if(symbol_table[index].argument_type != argument_type && 
           symbol_table[index].argument_type != NONE_TYPE && 
           argument_type != NONE_TYPE) {
            printf("\nFunction Argument Type Mismatch error!!!!");
            // Don't exit
        }
    }
#line 1896 "parser.tab.c"
    break;

  case 24: /* function_call: function_name LPAREN RPAREN TERMINATOR_SYMBOL  */
#line 609 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION_CALL, (yyvsp[-3].str));
        int index = isFunctionDeclared((yyvsp[-3].str));
        if (index == -1) {
            printf("\nFunction %s is not declared!!!!\n", (yyvsp[-3].str));
            // Don't exit
        } else if(symbol_table[index].param_count != 0) {
            printf("\nFunction %s Argument Mismatch error!!!! Expected %d arguments, got 0.\n\n", 
                  (yyvsp[-3].str), symbol_table[index].param_count);
            // Don't exit
        }
    }
#line 1913 "parser.tab.c"
    break;

  case 25: /* function_name: IDENTIFIER  */
#line 625 "parser.y"
    {
        (yyval.str) = (yyvsp[0].id);
    }
#line 1921 "parser.tab.c"
    break;

  case 26: /* argument_list: argument  */
#line 632 "parser.y"
    {
        (yyval.num) = 1;
    }
#line 1929 "parser.tab.c"
    break;

  case 27: /* argument_list: argument_list COMMA argument  */
#line 636 "parser.y"
    {
        (yyval.num) = (yyvsp[-2].num) + 1;
    }
#line 1937 "parser.tab.c"
    break;

  case 28: /* argument: IDENTIFIER  */
#line 643 "parser.y"
    {
        int argumentIndex = isVariableDeclared((yyvsp[0].id), current_function_name);
        if (argumentIndex == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[0].id));
            // Don't exit
        } else {
            argument_type = symbol_table[argumentIndex].type;
        }
    }
#line 1951 "parser.tab.c"
    break;

  case 29: /* statement_list: statement  */
#line 656 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_PROGRAM, "statement_list");
        if ((yyval.ast) && (yyvsp[0].ast)) add_ast_child((yyval.ast), (yyvsp[0].ast));
    }
#line 1960 "parser.tab.c"
    break;

  case 30: /* statement_list: statement_list statement  */
#line 661 "parser.y"
    {
        if ((yyvsp[-1].ast) && (yyvsp[0].ast)) add_ast_child((yyvsp[-1].ast), (yyvsp[0].ast));
        (yyval.ast) = (yyvsp[-1].ast);
    }
#line 1969 "parser.tab.c"
    break;

  case 31: /* statement_list: %empty  */
#line 666 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_PROGRAM, "empty_statement_list");
    }
#line 1977 "parser.tab.c"
    break;

  case 32: /* statement: for_loop  */
#line 673 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 1985 "parser.tab.c"
    break;

  case 33: /* statement: while_loop  */
#line 677 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 1993 "parser.tab.c"
    break;

  case 34: /* statement: stack_top  */
#line 681 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2001 "parser.tab.c"
    break;

  case 35: /* statement: stack_pop  */
#line 685 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2009 "parser.tab.c"
    break;

  case 36: /* statement: stack_declaration_stmt  */
#line 689 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2017 "parser.tab.c"
    break;

  case 37: /* statement: stack_push  */
#line 693 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2025 "parser.tab.c"
    break;

  case 38: /* statement: function_call  */
#line 697 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2033 "parser.tab.c"
    break;

  case 39: /* statement: return_statement  */
#line 701 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2041 "parser.tab.c"
    break;

  case 40: /* statement: variable_declaration  */
#line 705 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2049 "parser.tab.c"
    break;

  case 41: /* statement: cout_statement  */
#line 709 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2057 "parser.tab.c"
    break;

  case 42: /* cout_statement: COUT OPERATOR STRING_LITERAL OPERATOR ENDL TERMINATOR_SYMBOL  */
#line 716 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_COUT, (yyvsp[-3].str));
        printf("Processed cout statement.\n");
    }
#line 2066 "parser.tab.c"
    break;

  case 43: /* cout_statement: COUT OPERATOR IDENTIFIER OPERATOR ENDL TERMINATOR_SYMBOL  */
#line 721 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_COUT, (yyvsp[-3].id));
        printf("Processed cout statement.\n");
    }
#line 2075 "parser.tab.c"
    break;

  case 44: /* variable_declaration: variable_declaration_name TERMINATOR_SYMBOL  */
#line 729 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, (yyvsp[-1].str));
        symbol_table[symbol_count - 1].param_count = 0;
    }
#line 2084 "parser.tab.c"
    break;

  case 45: /* variable_declaration: variable_declaration_name ASSIGNMENT_OPERATOR variable_value TERMINATOR_SYMBOL  */
#line 734 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, (yyvsp[-3].str));
        symbol_table[symbol_count - 1].param_count = 0;
    }
#line 2093 "parser.tab.c"
    break;

  case 46: /* variable_value: NUMBER  */
#line 742 "parser.y"
    {
        if((yyvsp[0].num) > INT_MAX || (yyvsp[0].num) < INT_MIN) {
            printf("\nInteger Range Exceeded!!!!!\n\n");
        }
        symbol_table[symbol_count - 1].val_type = STATIC;
        char buffer[32];
        sprintf(buffer, "%d", (yyvsp[0].num));
        (yyval.str) = strdup(buffer);
    }
#line 2107 "parser.tab.c"
    break;

  case 47: /* variable_value: STRING_LITERAL  */
#line 752 "parser.y"
    {
        symbol_table[symbol_count - 1].val_type = STATIC;
        (yyval.str) = (yyvsp[0].str);
    }
#line 2116 "parser.tab.c"
    break;

  case 48: /* variable_value: stack_size  */
#line 757 "parser.y"
    {
        if((yyvsp[0].num) > INT_MAX || (yyvsp[0].num) < INT_MIN) {
            printf("\nInteger Range Exceeded!!!!!\n\n");
        }
        symbol_table[symbol_count - 1].val_type = STACK_SIZE;
        char buffer[32];
        sprintf(buffer, "%d", (yyvsp[0].num));
        (yyval.str) = strdup(buffer);
    }
#line 2130 "parser.tab.c"
    break;

  case 49: /* for_loop_signature: FOR LPAREN initialization_block TERMINATOR_SYMBOL condition_block TERMINATOR_SYMBOL action_block RPAREN  */
#line 770 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FOR_LOOP, "for_loop_signature");
        if ((yyval.ast)) {
            if ((yyvsp[-5].ast)) add_ast_child((yyval.ast), (yyvsp[-5].ast));
            if ((yyvsp[-3].ast)) add_ast_child((yyval.ast), (yyvsp[-3].ast));
            if ((yyvsp[-1].ast)) add_ast_child((yyval.ast), (yyvsp[-1].ast));
        }
    }
#line 2143 "parser.tab.c"
    break;

  case 50: /* while_loop: WHILE LPAREN while_condition RPAREN LBRACE statement_list RBRACE  */
#line 782 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_WHILE_LOOP, "while_loop");
        if ((yyval.ast)) {
            if ((yyvsp[-4].ast)) add_ast_child((yyval.ast), (yyvsp[-4].ast));
            if ((yyvsp[-1].ast)) add_ast_child((yyval.ast), (yyvsp[-1].ast));
        }
        printf("Processed while loop statement.\n");
    }
#line 2156 "parser.tab.c"
    break;

  case 51: /* while_condition: NOT_EQUAL IDENTIFIER DOT EMPTY LPAREN RPAREN  */
#line 794 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_OP, "not_empty");
        if (isVariableDeclared((yyvsp[-4].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-4].id));
            // Don't exit
        }
    }
#line 2168 "parser.tab.c"
    break;

  case 52: /* while_condition: IDENTIFIER DOT EMPTY LPAREN RPAREN  */
#line 802 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_OP, "empty");
        if (isVariableDeclared((yyvsp[-4].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-4].id));
            // Don't exit
        }
    }
#line 2180 "parser.tab.c"
    break;

  case 53: /* while_condition: condition_block  */
#line 810 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2188 "parser.tab.c"
    break;

  case 54: /* initialization_block: INT IDENTIFIER ASSIGNMENT_OPERATOR NUMBER  */
#line 817 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, (yyvsp[-2].id));
        add_symbol((yyvsp[-2].id), INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
    }
#line 2199 "parser.tab.c"
    break;

  case 55: /* initialization_block: IDENTIFIER ASSIGNMENT_OPERATOR NUMBER  */
#line 824 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, (yyvsp[-2].id));
        if (isVariableDeclared((yyvsp[-2].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-2].id));
            // Don't exit
        }
    }
#line 2211 "parser.tab.c"
    break;

  case 56: /* initialization_block: %empty  */
#line 832 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "empty_init");
    }
#line 2219 "parser.tab.c"
    break;

  case 57: /* condition_block: IDENTIFIER SPECIAL_SYMBOL IDENTIFIER  */
#line 839 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "condition");
        if (isVariableDeclared((yyvsp[-2].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-2].id));
            // Don't exit
        }
        if (isVariableDeclared((yyvsp[0].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[0].id));
            // Don't exit
        }
    }
#line 2235 "parser.tab.c"
    break;

  case 58: /* condition_block: IDENTIFIER ASSIGNMENT_OPERATOR IDENTIFIER  */
#line 851 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "condition_assign");
        if (isVariableDeclared((yyvsp[-2].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-2].id));
            // Don't exit
        }
        if (isVariableDeclared((yyvsp[0].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[0].id));
            // Don't exit
        }
    }
#line 2251 "parser.tab.c"
    break;

  case 59: /* condition_block: %empty  */
#line 863 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "empty_condition");
    }
#line 2259 "parser.tab.c"
    break;

  case 60: /* action_block: IDENTIFIER INCREMENT_OPERATOR INCREMENT_OPERATOR  */
#line 870 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "increment");
        if (isVariableDeclared((yyvsp[-2].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", (yyvsp[-2].id));
            // Don't exit
        }
    }
#line 2271 "parser.tab.c"
    break;

  case 61: /* action_block: %empty  */
#line 878 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_VARIABLE_DECL, "empty_action");
    }
#line 2279 "parser.tab.c"
    break;

  case 62: /* for_loop: for_loop_signature LBRACE statement_list RBRACE  */
#line 885 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FOR_LOOP, "for_loop");
        if ((yyval.ast)) {
            if ((yyvsp[-3].ast)) add_ast_child((yyval.ast), (yyvsp[-3].ast));
            if ((yyvsp[-1].ast)) add_ast_child((yyval.ast), (yyvsp[-1].ast));
        }
    }
#line 2291 "parser.tab.c"
    break;

  case 63: /* stack_declaration_stmt: stack_declaration  */
#line 896 "parser.y"
    {
        (yyval.ast) = (yyvsp[0].ast);
    }
#line 2299 "parser.tab.c"
    break;

  case 64: /* stack_declaration: STACK SPECIAL_SYMBOL INT SPECIAL_SYMBOL IDENTIFIER TERMINATOR_SYMBOL  */
#line 903 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_DECL, (yyvsp[-1].id));
        add_symbol((yyvsp[-1].id), STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;

        add_stack((yyvsp[-1].id), "INT", current_function_name, true);
    }
#line 2312 "parser.tab.c"
    break;

  case 65: /* stack_declaration: STACK SPECIAL_SYMBOL CHARSTAR SPECIAL_SYMBOL IDENTIFIER TERMINATOR_SYMBOL  */
#line 912 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_DECL, (yyvsp[-1].id));
        add_symbol((yyvsp[-1].id), STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;

        add_stack((yyvsp[-1].id), "STRING", current_function_name, true);
    }
#line 2325 "parser.tab.c"
    break;

  case 66: /* stack_push: IDENTIFIER DOT PUSH LPAREN variable_value RPAREN TERMINATOR_SYMBOL  */
#line 924 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_OP, "push");
        if (isVariableDeclared((yyvsp[-6].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", (yyvsp[-6].id));
            // Don't exit
        } else {
            stack_push_ele((yyvsp[-6].id), get_data_type_from_value((yyvsp[-2].str)), current_function_name);
            stack_typemismatch((yyvsp[-6].id), current_function_name, (yyvsp[-2].str));
        }
        printf("Processed stack push statement.\n");
    }
#line 2341 "parser.tab.c"
    break;

  case 67: /* stack_size: IDENTIFIER DOT SIZE LPAREN RPAREN  */
#line 939 "parser.y"
    {
        if (isVariableDeclared((yyvsp[-4].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", (yyvsp[-4].id));
            // Don't exit
        }
        printf("Processed stack size statement.\n");
        (yyval.num) = 0; // Return a default value
    }
#line 2354 "parser.tab.c"
    break;

  case 68: /* stack_pop: IDENTIFIER DOT POP LPAREN RPAREN TERMINATOR_SYMBOL  */
#line 951 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_OP, "pop");
        if (isVariableDeclared((yyvsp[-5].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", (yyvsp[-5].id));
            // Don't exit
        } else {
            stack_pop_ele((yyvsp[-5].id), current_function_name);
        }
        printf("Processed stack pop statement.\n");
    }
#line 2369 "parser.tab.c"
    break;

  case 69: /* stack_top: COUT OPERATOR IDENTIFIER DOT TOP LPAREN RPAREN OPERATOR ENDL TERMINATOR_SYMBOL  */
#line 965 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_STACK_OP, "top");
        if (isVariableDeclared((yyvsp[-7].id), current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", (yyvsp[-7].id));
            // Don't exit
        } else {
            stack_top_ele((yyvsp[-7].id), current_function_name);
        }
        printf("Processed stack top statement.\n");
    }
#line 2384 "parser.tab.c"
    break;

  case 70: /* main_function: main_function_signature main_function_definition  */
#line 979 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "main");
        if ((yyval.ast)) {
            if ((yyvsp[-1].ast)) add_ast_child((yyval.ast), (yyvsp[-1].ast));
            if ((yyvsp[0].ast)) add_ast_child((yyval.ast), (yyvsp[0].ast));
        }
        printf("main function declaration\n");
    }
#line 2397 "parser.tab.c"
    break;

  case 71: /* main_function_signature: INT MAIN LPAREN RPAREN  */
#line 991 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_FUNCTION, "main_signature");
        current_function_name = strdup("main");
        add_symbol("main", INT_TYPE, "main", true); 
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = NONE;
        symbol_table[symbol_count - 1].argument_type = NONE_TYPE;
    }
#line 2410 "parser.tab.c"
    break;

  case 72: /* main_function_definition: LBRACE statement_list RBRACE  */
#line 1003 "parser.y"
    {
        (yyval.ast) = (yyvsp[-1].ast);
    }
#line 2418 "parser.tab.c"
    break;

  case 73: /* return_statement: RETURN NUMBER TERMINATOR_SYMBOL  */
#line 1010 "parser.y"
    {
        char buffer[32];
        sprintf(buffer, "%d", (yyvsp[-1].num));
        (yyval.ast) = create_ast_node(AST_RETURN, buffer);
    }
#line 2428 "parser.tab.c"
    break;

  case 74: /* return_statement: RETURN TERMINATOR_SYMBOL  */
#line 1016 "parser.y"
    {
        (yyval.ast) = create_ast_node(AST_RETURN, "void");
        printf("Processed return statement.\n");
    }
#line 2437 "parser.tab.c"
    break;


#line 2441 "parser.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 1022 "parser.y"


int main(void) {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    yyin = file;

    printf("\n====*Parser Comments*====\n\n");

    yyparse();

    printf("\n\n====*Tokens Found*=====\n\n");
    print_tokens();

    printf("\n\n====*Variables & Functions*=====\n\n");
    show_symbol_table();
    
    printf("\n\n====*Abstract Syntax Tree*=====\n\n");
    if (ast_root) {
        print_ast(ast_root, 0);
        free_ast(ast_root);
    } else {
        printf("No AST was generated.\n");
    }
    
    // Clean up
    fclose(file);

    return 0;
}

int yyerror(const char *s) {
    printf("Error: %s\n\n", s);
    return 0;
}
