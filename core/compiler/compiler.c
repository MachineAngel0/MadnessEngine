#include "compiler.h"

void lexer_test()
{
    Lexer* lexer = lexer_init();
    lexer_generate_tokens(lexer, "../core/compiler/compiler_test_file.h");

    Lexer* lexer2 = lexer_init();
    lexer_generate_tokens(lexer2, "../core/compiler/shader_test_file.madnessshader");
    u64 token_size = darray_get_size(lexer2->tokens);
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



void lexer_free(Lexer* lexer)
{
    darray_free(lexer->tokens);
    free(lexer);
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

    fclose(file);

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
    DEBUG("token size: %d", size)


    //have to merge any number values into one value
}

Token* lexer_prune_tokens(Token* token_array, Token_Type* tokens_to_remove, u32 list_size)
{
    Token* out_list = darray_create_reserve(Token, darray_get_size(token_array));
    u64 size = darray_get_size(token_array);
    for (u64 i = 0; i < size; i++)
    {
        for (u64 list_idx = 0; list_idx < list_size; list_idx++)
        {
            if (tokens_to_remove[list_idx] == token_array[i].type)
            {
                darray_push(out_list, token_array[i]);
                break;
            }
        }
    }

    return out_list;
}

bool lexer_is_token_data_type(Token token)
{
    switch (token.type)
    {
    case Token_VAR:
        return true;
        break;
    case Token_U8:
        return true;
        break;
    case Token_U16:
        return true;
        break;
    case Token_U32:
        return true;
        break;
    case Token_U64:
        return true;
        break;
    case Token_I8:
        return true;
        break;
    case Token_I16:
        return true;
        break;
    case Token_I32:
        return true;
        break;
    case Token_I64:
        return true;
        break;
    case Token_F32:
        return true;
        break;
    case Token_F64:
        return true;
        break;
    case Token_char:
        return true;
        break;
    case Token_size_t:
        return true;
        break;
    case Token_bool:
        return true;
        break;
    case Token_string_type:
        return true;
        break;

    default:
        return false;
        break;
    }
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
    string_builder_append_char(&token->string_builder, *lexer->position);
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

            string_builder_append_char(&token->string_builder, *lexer->position);

            while (lexer->position[0] &&
                lexer->position[0] != '"')
            {
                if ((lexer->position[0] == '\\') &&
                    lexer->position[1])
                {
                    ++lexer->position;
                    string_builder_append_char(&token->string_builder, *lexer->position);
                }
                ++lexer->position;
                string_builder_append_char(&token->string_builder, *lexer->position);
            }

            // Saves total string length
            // token->text.length = lexer->position - token->text.str;

            if (lexer->position[0] == '"')
            {
                ++lexer->position;
                string_builder_append_char(&token->string_builder, *lexer->position);
            }
        }
        break;
    case '#':
        {
            token->type = Token_MACRO;

            string_builder_append_char(&token->string_builder, *lexer->position);

            while (lexer->position[0] && lexer->position[0] != ' ' &&
                lexer->position[0] != '\n')
            {
                ++lexer->position;
                string_builder_append_char(&token->string_builder, *lexer->position);
            }
            // Saves total string length
            // token->text.length = lexer->position - token->text.str;
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
                    string_builder_append_char(&token->string_builder, *lexer->position);

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
                    token->type = Token_U8;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u16")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint16_t"))
                {
                    token->type = Token_U16;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint32_t"))
                {
                    token->type = Token_U32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint64_t"))
                {
                    token->type = Token_U32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "i8")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int8_t"))
                {
                    token->type = Token_I8;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "i16")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int16_t"))
                {
                    token->type = Token_I16;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int32_t")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "int"))
                {
                    token->type = Token_I32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "u64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "uint64_t"))
                {
                    token->type = Token_I64;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "f32")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "float"))
                {
                    token->type = Token_F32;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "f64")
                    || STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "double"))
                {
                    token->type = Token_F64;
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
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "String"))
                {
                    token->type = Token_string_type;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "const"))
                {
                    token->type = Token_const;
                }
                if (STRING_BUILDER_COMPARE_WITH_CHAR(&token->string_builder, "BITFLAG"))
                {
                    token->type = Token_BITFLAG;
                }
            } // Numbers
            else if (IsNumber(c))
            {
                token->type = Token_Number;
                //this loop is here so we can get all the number values and not just the individual value
                while (lexer->position[0] &&  IsNumber(lexer->position[0]) && lexer->position[0] != ' ' &&
                    lexer->position[0] != '\n')
                {
                    string_builder_append_char(&token->string_builder, *lexer->position);
                    ++lexer->position;
                }
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
