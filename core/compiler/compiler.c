#include "compiler.h"

void lexer_test()
{
    Lexer* lexer = lexer_init();
    lexer_generate_tokens(lexer, "../core/compiler/compiler_test_file.h");

    Lexer* lexer2 = lexer_init();
    lexer_generate_tokens(lexer, "../core/compiler/shader_test_file.madnessshader");


}

Lexer* lexer_init()
{
    Lexer* lexer = malloc(sizeof(Lexer));

    lexer->position = NULL;
    lexer->line = 0;
    lexer->column = 0;

    lexer->error = false;
    lexer->error_line = 0;
    lexer->tokens = darray_create_reserve(Token, 100);

    return lexer;
}

void lexer_generate_tokens(Lexer* lexer, const char* file_path)
{
    MASSERT(lexer);

    FILE* file = fopen(file_path, "r");
    MASSERT(file);

    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fseek(file, 0L, SEEK_SET);

    lexer->position = malloc(sz);
    fread(lexer->position, 1, sz, file);


    while (true)
    {
        Token token = {0};
        token.type = Token_Unknown;

        nextToken(lexer, &token);
        darray_push(lexer->tokens, token);

        if (token.type == Token_EndOfStream)
        {
            break;
        }
    }

    u64 size = darray_get_size(lexer->tokens);
    u64 sizea = darray_get_size(lexer->tokens);
}


void skipWhitespace(Lexer* lexer)
{
    // Scan text until whitespace is finished.
    for (;;)
    {
        // Check if it is a pure whitespace first.
        if (IsWhitespace(lexer->position[0]))
        {
            // Handle change of line
            if (IsEndOfLine(lexer->position[0]))
                ++lexer->line;

            // Advance to next character
            ++lexer->position;
        } // Check for single line comments ("//")
        else if ((lexer->position[0] == '/') && (lexer->position[1] == '/'))
        {
            lexer->position += 2;
            while (lexer->position[0] && !IsEndOfLine(lexer->position[0]))
            {
                ++lexer->position;
            }
        } // Check for c-style multi-lines comments
        else if ((lexer->position[0] == '/') && (lexer->position[1] == '*'))
        {
            lexer->position += 2;

            // Advance until the string is closed. Remember to check if line is changed.
            while (!((lexer->position[0] == '*') && (lexer->position[1] == '/')))
            {
                // Handle change of line
                if (IsEndOfLine(lexer->position[0]))
                    ++lexer->line;

                // Advance to next character
                ++lexer->position;
            }

            if (lexer->position[0] == '*')
            {
                lexer->position += 2;
            }
        }
        else
        {
            break;
        }
    }
}

void nextToken(Lexer* lexer, Token* token)
{
    // Skip all whitespace first so that the token is without them.
    skipWhitespace(lexer);

    // Initialize token
    token->type = Token_Unknown;
    string_builder_append_single_char(&token->string_builder, lexer->position);
    token->line = lexer->line;

    char c = lexer->position[0];
    ++lexer->position;

    switch (c)
    {
    case '\0':
        {
            token->type = Token_EndOfStream;
        }
        break;
    case '(':
        {
            token->type = Token_OpenParen;
        }
        break;
    case ')':
        {
            token->type = Token_CloseParen;
        }
        break;
    case ':':
        {
            token->type = Token_Colon;
        }
        break;
    case ';':
        {
            token->type = Token_Semicolon;
        }
        break;
    case '*':
        {
            token->type = Token_Asterisk;
        }
        break;
    case '[':
        {
            token->type = Token_OpenBracket;
        }
        break;
    case ']':
        {
            token->type = Token_CloseBracket;
        }
        break;
    case '{':
        {
            token->type = Token_OpenBrace;
        }
        break;
    case '}':
        {
            token->type = Token_CloseBrace;
        }
        break;
    case '"':
        {
            token->type = Token_String;

            string_builder_append_single_char(&token->string_builder, lexer->position);

            while (lexer->position[0] &&
                lexer->position[0] != '"')
            {
                if ((lexer->position[0] == '\\') &&
                    lexer->position[1])
                {
                    ++lexer->position;
                    string_builder_append_single_char(&token->string_builder, lexer->position);
                }
                ++lexer->position;
                string_builder_append_single_char(&token->string_builder, lexer->position);
            }

            // Saves total string length
            // token->text.length = lexer->position - token->text.str;

            if (lexer->position[0] == '"')
            {
                ++lexer->position;
                string_builder_append_single_char(&token->string_builder, lexer->position);
            }
        }
        break;
    default:
        {
            // Identifier/keywords
            if (IsAlpha(c))
            {
                token->type = Token_Identifier;

                while (IsAlpha(lexer->position[0]) || IsNumber(lexer->position[0]) || (lexer->position[0] == '_'))
                {
                    string_builder_append_single_char(&token->string_builder, lexer->position);

                    ++lexer->position;
                }

                // check if it matches a keyword exactly

                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "typedef"))
                {
                    token->type = Token_Typedef;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "enum"))
                {
                    token->type = Token_Enum;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "struct"))
                {
                    token->type = Token_Struct;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "&&"))
                {
                    token->type = Token_And;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "if"))
                {
                    token->type = Token_If;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "else"))
                {
                    token->type = Token_Else;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "false"))
                {
                    token->type = Token_False;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "true"))
                {
                    token->type = Token_TRUE;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "while"))
                {
                    token->type = Token_WHILE;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "for"))
                {
                    token->type = Token_For;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "return"))
                {
                    token->type = Token_RETURN;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "NULL"))
                {
                    token->type = Token_NULL;
                }

                // types
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u8")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint8_t"))
                {
                    token->type = Token_u8;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u16")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint16_t"))
                {
                    token->type = Token_u16;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint32_t"))
                {
                    token->type = Token_u32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint64_t"))
                {
                    token->type = Token_u32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "i8")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int8_t"))
                {
                    token->type = Token_i8;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "i16")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int16_t"))
                {
                    token->type = Token_i16;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int32_t")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int"))
                {
                    token->type = Token_i32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint64_t"))
                {
                    token->type = Token_i64;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "f32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "float"))
                {
                    token->type = Token_f32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "f64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "double"))
                {
                    token->type = Token_f64;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "size_t"))
                {
                    token->type = Token_size_t;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "char"))
                {
                    token->type = Token_char;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "bool"))
                {
                    token->type = Token_bool;
                }

                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "const"))
                {
                    token->type = Token_const;
                }
            } // Numbers
            else if (IsNumber(c))
            {
                token->type = Token_Number;
            }
            else
            {
                token->type = Token_Unknown;
            }
        }
        break;
    }
}


bool IsEndOfLine(char c)
{
    bool Result = ((c == '\n') || (c == '\r'));
    return (Result);
}

bool IsWhitespace(char c)
{
    bool Result = ((c == ' ') || (c == '\t') || (c == '\v') || (c == '\f') || IsEndOfLine(c));
    return (Result);
}

bool IsAlpha(char c)
{
    bool Result = (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
    return (Result);
}

bool IsNumber(char c)
{
    bool Result = ((c >= '0') && (c <= '9'));
    return (Result);
}
