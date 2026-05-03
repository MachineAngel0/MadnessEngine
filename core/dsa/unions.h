#ifndef UNIONS_H
#define UNIONS_H


// --- Type Tag ---
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,  // dynamically allocated copy
    TYPE_CUSTOM,   // for structs
} Union_Type;

// --- Tagged Union ---
typedef struct {
    Union_Type type;
    union {
        int i;
        float f;
        char c;
        char* s;
        void* custom;   // generic pointer to a struct or custom data
    } data;
} Tagged_Union;


#endif //UNIONS_H
