#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "../dsa/darray.h"
#include "str_builder.h"


//up to who ever is using this to generate the code they want
// typedef struct Code_Generation
// {
// } Code_Generation;

typedef enum Token_Type
{
    Token_Unknown,

    //single char tokens
    Token_OpenParen,
    Token_CloseParen,
    Token_Colon,
    Token_Semicolon,
    Token_Asterisk,
    Token_OpenBracket,
    Token_CloseBracket,
    Token_OpenBrace,
    Token_CloseBrace,
    Token_OpenAngleBracket,
    Token_CloseAngleBracket,


    // One or two character tokens. // operators
    // TOKEN_EXCLAMATION, TOKEN_EXCLAMATION_EQUAL,
    // TOKEN_EQUAL, Token_EQUAL_EQUAL,
    // Token_GREATER, Token_GREATER_EQUAL,
    // Token_LESS, Token_LESS_EQUAL,

    // Token_ADD,
    // Token_SUB,
    // Token_MUL,
    // Token_DIV,


    // Literals.
    Token_MACRO,
    Token_String,
    Token_Identifier,
    Token_Number,

    //keywords
    Token_Typedef,
    Token_Enum,
    Token_Struct,

    Token_OR,
    Token_And,
    Token_If,
    Token_Else,

    Token_False,
    Token_TRUE,

    Token_NULL,
    Token_WHILE,
    Token_For,
    Token_RETURN,
    Token_VAR,

    //Types
    Token_U8,
    Token_U16,
    Token_U32,
    Token_U64,
    Token_I8,
    Token_I16,
    Token_I32,
    Token_I64,
    Token_F32,
    Token_F64,
    Token_char,
    Token_size_t,
    Token_bool,
    Token_string_type,//my custom type


    Token_const,

    Token_EndOfStream,
    Token_MAX,
} Token_Type;


typedef struct Token
{
    String_Builder string_builder;
    Token_Type type;
    u32 line;
} Token;

typedef struct Lexer
{
    char* position;
    uint32_t line;
    uint32_t column;

    bool error;
    uint32_t error_line;

    darray_type(Token)* tokens;
} Lexer;


typedef struct Parser
{
    bool unimplemented;
} Parser;

Lexer* lexer_init();
void lexer_generate_tokens(Lexer* lexer, const char* file_path);
void lexer_test();

Token* lexer_prune_tokens(Token* token_array, Token_Type* tokens_to_remove, u32 list_size);

bool lexer_is_token_data_type(Token token);

void skipWhitespace(Lexer* lexer);

void nextToken(Lexer* lexer, Token* token);

bool IsEndOfLine(char c);

bool IsWhitespace(char c);

bool IsAlpha(char c);

bool IsNumber(char c);


#endif //COMPILER_H
