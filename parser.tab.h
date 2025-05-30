/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    PREPROCESSOR = 258,            /* PREPROCESSOR  */
    HEADER_FILE = 259,             /* HEADER_FILE  */
    SPECIAL_SYMBOL = 260,          /* SPECIAL_SYMBOL  */
    USING = 261,                   /* USING  */
    NAMESPACE = 262,               /* NAMESPACE  */
    STD = 263,                     /* STD  */
    INT = 264,                     /* INT  */
    VOID = 265,                    /* VOID  */
    MAIN = 266,                    /* MAIN  */
    CHARSTAR = 267,                /* CHARSTAR  */
    IF = 268,                      /* IF  */
    ELSE = 269,                    /* ELSE  */
    FOR = 270,                     /* FOR  */
    ASSIGNMENT_OPERATOR = 271,     /* ASSIGNMENT_OPERATOR  */
    INCREMENT_OPERATOR = 272,      /* INCREMENT_OPERATOR  */
    RETURN = 273,                  /* RETURN  */
    COUT = 274,                    /* COUT  */
    ENDL = 275,                    /* ENDL  */
    OPERATOR = 276,                /* OPERATOR  */
    STACK = 277,                   /* STACK  */
    SIZE = 278,                    /* SIZE  */
    POP = 279,                     /* POP  */
    PUSH = 280,                    /* PUSH  */
    EMPTY = 281,                   /* EMPTY  */
    TOP = 282,                     /* TOP  */
    DOT = 283,                     /* DOT  */
    LPAREN = 284,                  /* LPAREN  */
    RPAREN = 285,                  /* RPAREN  */
    LBRACE = 286,                  /* LBRACE  */
    RBRACE = 287,                  /* RBRACE  */
    LBRACKET = 288,                /* LBRACKET  */
    RBRACKET = 289,                /* RBRACKET  */
    TERMINATOR_SYMBOL = 290,       /* TERMINATOR_SYMBOL  */
    COMMA = 291,                   /* COMMA  */
    WHILE = 292,                   /* WHILE  */
    NOT_EQUAL = 293,               /* NOT_EQUAL  */
    IDENTIFIER = 294,              /* IDENTIFIER  */
    STRING_LITERAL = 295,          /* STRING_LITERAL  */
    NUMBER = 296                   /* NUMBER  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 385 "parser.y"

    int num;
    char ch;
    char* str;
    char* id;
    struct AstNode* ast;

#line 113 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
