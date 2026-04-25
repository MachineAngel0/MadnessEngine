#ifndef COMPILER_H
#define COMPILER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "darray.h"
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

    Token_u8,
    Token_u16,
    Token_u32,
    Token_u64,
    Token_i8,
    Token_i16,
    Token_i32,
    Token_i64,
    Token_f32,
    Token_f64,
    Token_char,
    Token_size_t,
    Token_bool,
    Token_const,

    Token_EndOfStream,
} Token_Type;


typedef struct Token
{
    Token_Type type;
    String_Builder string_builder;
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


void skipWhitespace(Lexer* lexer);

void nextToken(Lexer* lexer, Token* token);

bool IsEndOfLine(char c);

bool IsWhitespace(char c);

bool IsAlpha(char c);

bool IsNumber(char c);


#endif //COMPILER_H
