%{
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

%}

%union {
    int num;
    char ch;
    char* str;
    char* id;
    struct AstNode* ast;
}

/* Define precedence and associativity to resolve shift/reduce conflicts */
%left OPERATOR
%left ASSIGNMENT_OPERATOR
%left DOT
%left SPECIAL_SYMBOL

%token PREPROCESSOR HEADER_FILE SPECIAL_SYMBOL
%token USING NAMESPACE STD
%token INT VOID MAIN CHARSTAR
%token IF ELSE FOR ASSIGNMENT_OPERATOR INCREMENT_OPERATOR
%token RETURN COUT ENDL OPERATOR
%token STACK SIZE POP PUSH EMPTY TOP DOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token TERMINATOR_SYMBOL COMMA
%token WHILE NOT_EQUAL
%token <id> IDENTIFIER
%token <str> STRING_LITERAL 
%token <num> NUMBER

%type <num> function_parameter_list argument_list stack_size
%type <str> variable_declaration_name function_name variable_value
%type <ast> program preprocessor_statement_list preprocessor_statement
%type <ast> std_declaration function_list function function_signature
%type <ast> function_declaration_name function_parameter function_definition
%type <ast> statement_list statement variable_declaration stack_declaration
%type <ast> stack_push stack_pop stack_top while_loop for_loop cout_statement
%type <ast> function_call return_statement main_function main_function_signature
%type <ast> main_function_definition stack_declaration_stmt
%type <ast> condition_block initialization_block action_block while_condition for_loop_signature

%%

program:
    preprocessor_statement_list std_declaration function_list main_function
    {
        printf("Program processed.\n");
        ast_root = create_ast_node(AST_PROGRAM, "program");
        if (ast_root) {
            add_ast_child(ast_root, $1);
            add_ast_child(ast_root, $2);
            add_ast_child(ast_root, $3);
            add_ast_child(ast_root, $4);
        }
        $$ = ast_root;
    };

preprocessor_statement_list:
    preprocessor_statement 
    {
        $$ = create_ast_node(AST_PREPROCESSOR, "preprocessor_list");
        if ($$) add_ast_child($$, $1);
        printf("Preprocessor statement processed.\n");
    }
    | preprocessor_statement_list preprocessor_statement
    {
        if ($1 && $2) add_ast_child($1, $2);
        $$ = $1;
        printf("Preprocessor statement_list processed.\n");
    };

preprocessor_statement:
    PREPROCESSOR SPECIAL_SYMBOL HEADER_FILE SPECIAL_SYMBOL 
    {
        $$ = create_ast_node(AST_PREPROCESSOR, "include");
        printf("Processed preprocessor statement.\n");
    }
    | PREPROCESSOR SPECIAL_SYMBOL STACK SPECIAL_SYMBOL
    {
        $$ = create_ast_node(AST_PREPROCESSOR, "include_stack");
        printf("Processed preprocessor statement.\n");
    };

std_declaration:
    USING NAMESPACE STD TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STD_DECL, "std_declaration");
        printf("Processed std statement.\n");
    };

function_list:
    function
    {
        $$ = create_ast_node(AST_FUNCTION, "function_list");
        if ($$) add_ast_child($$, $1);
    }
    | function_list function
    {
        if ($1 && $2) add_ast_child($1, $2);
        $$ = $1;
    }
    | /* empty */
    {
        $$ = create_ast_node(AST_FUNCTION, "empty_function_list");
        printf("function_list processed.\n");
    };

function:
    function_signature function_definition
    {
        $$ = create_ast_node(AST_FUNCTION, "function");
        if ($$) {
            add_ast_child($$, $1);
            add_ast_child($$, $2);
        }
        printf("function processed.\n");
    };

function_signature:
    function_declaration_name LPAREN RPAREN
    {
        $$ = create_ast_node(AST_FUNCTION, "function_signature_no_params");
        if ($$) add_ast_child($$, $1);
        symbol_table[symbol_count - 1].param_count = 0;
        printf("function signature.\n");
    }
    | function_declaration_name LPAREN function_parameter_list RPAREN
    {
        $$ = create_ast_node(AST_FUNCTION, "function_signature_with_params");
        if ($$) add_ast_child($$, $1);
        symbol_table[symbol_count - 1].param_count = $3;
        printf("function signature with parameters.\n");
    };

function_declaration_name:
    VOID IDENTIFIER
    {
        current_function_name = strdup($2);
        add_symbol($2, VOID_TYPE, current_function_name, true); 
        symbol_table[symbol_count - 1].val_type = NONE;
        $$ = create_ast_node(AST_FUNCTION, $2);
        printf("void function_declaration_name: %s\n", $2);
    }
    | INT IDENTIFIER
    {
        current_function_name = strdup($2);
        add_symbol($2, INT_TYPE, current_function_name, true); 
        symbol_table[symbol_count - 1].val_type = NONE;
        $$ = create_ast_node(AST_FUNCTION, $2);
        printf("int function_declaration_name: %s\n", $2);
    };

variable_declaration_name:
    VOID IDENTIFIER
    {
        add_symbol($2, VOID_TYPE, current_function_name, false); 
        symbol_table[symbol_count - 1].param_count = 0;
        $$ = $2;
    }
    | INT IDENTIFIER
    {
        add_symbol($2, INT_TYPE, current_function_name, false); 
        symbol_table[symbol_count - 1].param_count = 0;
        $$ = $2;
    };

function_parameter_list:
    function_parameter 
    {
        $$ = 1;
    }
    | function_parameter_list COMMA function_parameter
    {
        $$ = $1 + 1;
    };

function_parameter:
    STACK SPECIAL_SYMBOL INT SPECIAL_SYMBOL IDENTIFIER 
    {
        printf("function parameter stack<int>\n");
        add_symbol($5, STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
        symbol_table[symbol_count - 2].argument_type = STACK_INT_TYPE;

        add_stack($5, "INT", current_function_name, false);
        $$ = create_ast_node(AST_STACK_DECL, $5);
    }
    | INT IDENTIFIER
    {
        add_symbol($2, INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
        symbol_table[symbol_count - 2].argument_type = INT_TYPE;
        $$ = create_ast_node(AST_VARIABLE_DECL, $2);
    }
    ;

function_definition:
    LBRACE statement_list RBRACE
    {
        $$ = $2;
        printf("function definition processed\n");
    };

function_call:
    function_name LPAREN argument_list RPAREN TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_FUNCTION_CALL, $1);
        int index = isFunctionDeclared($1);
        if (index == -1) {
            printf("\nFunction %s is not declared!!!!\n", $1);
            // Don't exit
        }
        else if(symbol_table[index].param_count != $3) {
            printf("\nFunction %s Argument Mismatch error!!!! Expected %d arguments, got %d.\n\n", 
                   $1, symbol_table[index].param_count, $3);
            // Don't exit
        }
        else if(symbol_table[index].argument_type != argument_type && 
           symbol_table[index].argument_type != NONE_TYPE && 
           argument_type != NONE_TYPE) {
            printf("\nFunction Argument Type Mismatch error!!!!");
            // Don't exit
        }
    }
    | function_name LPAREN RPAREN TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_FUNCTION_CALL, $1);
        int index = isFunctionDeclared($1);
        if (index == -1) {
            printf("\nFunction %s is not declared!!!!\n", $1);
            // Don't exit
        } else if(symbol_table[index].param_count != 0) {
            printf("\nFunction %s Argument Mismatch error!!!! Expected %d arguments, got 0.\n\n", 
                  $1, symbol_table[index].param_count);
            // Don't exit
        }
    }
    ;

function_name:
    IDENTIFIER
    {
        $$ = $1;
    }
    ;

argument_list:
    argument 
    {
        $$ = 1;
    }
    | argument_list COMMA argument
    {
        $$ = $1 + 1;
    }
    ;

argument:
    IDENTIFIER
    {
        int argumentIndex = isVariableDeclared($1, current_function_name);
        if (argumentIndex == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        } else {
            argument_type = symbol_table[argumentIndex].type;
        }
    }
    ;

statement_list:
    statement 
    {
        $$ = create_ast_node(AST_PROGRAM, "statement_list");
        if ($$ && $1) add_ast_child($$, $1);
    }
    | statement_list statement
    {
        if ($1 && $2) add_ast_child($1, $2);
        $$ = $1;
    }
    | /* empty */
    {
        $$ = create_ast_node(AST_PROGRAM, "empty_statement_list");
    }
    ;

statement: 
    for_loop
    {
        $$ = $1;
    }
    | while_loop
    {
        $$ = $1;
    }
    | stack_top
    {
        $$ = $1;
    }
    | stack_pop
    {
        $$ = $1;
    }
    | stack_declaration_stmt
    {
        $$ = $1;
    }
    | stack_push
    {
        $$ = $1;
    }
    | function_call
    {
        $$ = $1;
    }
    | return_statement
    {
        $$ = $1;
    }
    | variable_declaration
    {
        $$ = $1;
    }
    | cout_statement
    {
        $$ = $1;
    }
    ;

cout_statement:
    COUT OPERATOR STRING_LITERAL OPERATOR ENDL TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_COUT, $3);
        printf("Processed cout statement.\n");
    }
    | COUT OPERATOR IDENTIFIER OPERATOR ENDL TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_COUT, $3);
        printf("Processed cout statement.\n");
    }
    ;

variable_declaration:
    variable_declaration_name TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, $1);
        symbol_table[symbol_count - 1].param_count = 0;
    }
    | variable_declaration_name ASSIGNMENT_OPERATOR variable_value TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, $1);
        symbol_table[symbol_count - 1].param_count = 0;
    }
    ;

variable_value:
    NUMBER
    {
        if($1 > INT_MAX || $1 < INT_MIN) {
            printf("\nInteger Range Exceeded!!!!!\n\n");
        }
        symbol_table[symbol_count - 1].val_type = STATIC;
        char buffer[32];
        sprintf(buffer, "%d", $1);
        $$ = strdup(buffer);
    }
    | STRING_LITERAL
    {
        symbol_table[symbol_count - 1].val_type = STATIC;
        $$ = $1;
    }
    | stack_size
    {
        if($1 > INT_MAX || $1 < INT_MIN) {
            printf("\nInteger Range Exceeded!!!!!\n\n");
        }
        symbol_table[symbol_count - 1].val_type = STACK_SIZE;
        char buffer[32];
        sprintf(buffer, "%d", $1);
        $$ = strdup(buffer);
    }
    ;

for_loop_signature:
    FOR LPAREN initialization_block TERMINATOR_SYMBOL condition_block TERMINATOR_SYMBOL action_block RPAREN
    {
        $$ = create_ast_node(AST_FOR_LOOP, "for_loop_signature");
        if ($$) {
            if ($3) add_ast_child($$, $3);
            if ($5) add_ast_child($$, $5);
            if ($7) add_ast_child($$, $7);
        }
    }
    ;

while_loop:
    WHILE LPAREN while_condition RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_ast_node(AST_WHILE_LOOP, "while_loop");
        if ($$) {
            if ($3) add_ast_child($$, $3);
            if ($6) add_ast_child($$, $6);
        }
        printf("Processed while loop statement.\n");
    }
    ;

while_condition:
    NOT_EQUAL IDENTIFIER DOT EMPTY LPAREN RPAREN
    {
        $$ = create_ast_node(AST_STACK_OP, "not_empty");
        if (isVariableDeclared($2, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $2);
            // Don't exit
        }
    }
    | IDENTIFIER DOT EMPTY LPAREN RPAREN
    {
        $$ = create_ast_node(AST_STACK_OP, "empty");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        }
    }
    | condition_block
    {
        $$ = $1;
    }
    ;

initialization_block:
    INT IDENTIFIER ASSIGNMENT_OPERATOR NUMBER
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, $2);
        add_symbol($2, INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;
    }
    | IDENTIFIER ASSIGNMENT_OPERATOR NUMBER
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, $1);
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        }
    }
    | /* empty */
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "empty_init");
    }
    ;

condition_block:
    IDENTIFIER SPECIAL_SYMBOL IDENTIFIER
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "condition");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        }
        if (isVariableDeclared($3, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $3);
            // Don't exit
        }
    }
    | IDENTIFIER ASSIGNMENT_OPERATOR IDENTIFIER
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "condition_assign");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        }
        if (isVariableDeclared($3, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $3);
            // Don't exit
        }
    }
    | /* empty */
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "empty_condition");
    }
    ;

action_block:
    IDENTIFIER INCREMENT_OPERATOR INCREMENT_OPERATOR
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "increment");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n", $1);
            // Don't exit
        }
    }
    | /* empty */
    {
        $$ = create_ast_node(AST_VARIABLE_DECL, "empty_action");
    }
    ;

for_loop:
    for_loop_signature LBRACE statement_list RBRACE
    {
        $$ = create_ast_node(AST_FOR_LOOP, "for_loop");
        if ($$) {
            if ($1) add_ast_child($$, $1);
            if ($3) add_ast_child($$, $3);
        }
    }
    ;

stack_declaration_stmt:
    stack_declaration
    {
        $$ = $1;
    }
    ;

stack_declaration:
    STACK SPECIAL_SYMBOL INT SPECIAL_SYMBOL IDENTIFIER TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STACK_DECL, $5);
        add_symbol($5, STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;

        add_stack($5, "INT", current_function_name, true);
    }
    | STACK SPECIAL_SYMBOL CHARSTAR SPECIAL_SYMBOL IDENTIFIER TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STACK_DECL, $5);
        add_symbol($5, STACK_INT_TYPE, current_function_name, false);
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = STATIC;

        add_stack($5, "STRING", current_function_name, true);
    }
    ;

stack_push:
    IDENTIFIER DOT PUSH LPAREN variable_value RPAREN TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STACK_OP, "push");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", $1);
            // Don't exit
        } else {
            stack_push_ele($1, get_data_type_from_value($5), current_function_name);
            stack_typemismatch($1, current_function_name, $5);
        }
        printf("Processed stack push statement.\n");
    }
    ;
    
stack_size:
    IDENTIFIER DOT SIZE LPAREN RPAREN
    {
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", $1);
            // Don't exit
        }
        printf("Processed stack size statement.\n");
        $$ = 0; // Return a default value
    }
    ;
    
stack_pop:
    IDENTIFIER DOT POP LPAREN RPAREN TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STACK_OP, "pop");
        if (isVariableDeclared($1, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", $1);
            // Don't exit
        } else {
            stack_pop_ele($1, current_function_name);
        }
        printf("Processed stack pop statement.\n");
    }
    ;

stack_top:
    COUT OPERATOR IDENTIFIER DOT TOP LPAREN RPAREN OPERATOR ENDL TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_STACK_OP, "top");
        if (isVariableDeclared($3, current_function_name) == -1) {
            printf("\nVariable %s is not declared!!!!\n\n", $3);
            // Don't exit
        } else {
            stack_top_ele($3, current_function_name);
        }
        printf("Processed stack top statement.\n");
    }
    ;
    
main_function:
    main_function_signature main_function_definition
    {
        $$ = create_ast_node(AST_FUNCTION, "main");
        if ($$) {
            if ($1) add_ast_child($$, $1);
            if ($2) add_ast_child($$, $2);
        }
        printf("main function declaration\n");
    }
    ;

main_function_signature:
    INT MAIN LPAREN RPAREN
    {
        $$ = create_ast_node(AST_FUNCTION, "main_signature");
        current_function_name = strdup("main");
        add_symbol("main", INT_TYPE, "main", true); 
        symbol_table[symbol_count - 1].param_count = 0;
        symbol_table[symbol_count - 1].val_type = NONE;
        symbol_table[symbol_count - 1].argument_type = NONE_TYPE;
    }
    ;

main_function_definition:
    LBRACE statement_list RBRACE
    {
        $$ = $2;
    }
    ;

return_statement:
    RETURN NUMBER TERMINATOR_SYMBOL
    {
        char buffer[32];
        sprintf(buffer, "%d", $2);
        $$ = create_ast_node(AST_RETURN, buffer);
    }
    | RETURN TERMINATOR_SYMBOL
    {
        $$ = create_ast_node(AST_RETURN, "void");
        printf("Processed return statement.\n");
    }
    ;

%%

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