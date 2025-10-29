//
// Created by Adams Humbert on 9/21/2025.
//

#ifndef UNIONS_H
#define UNIONS_H


// --- Type Tag ---
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_STRING,  // dynamically allocated copy
    TYPE_CUSTOM,   // for structs
} ValueType;

// --- Tagged Union ---
typedef struct {
    ValueType type;
    union {
        int i;
        float f;
        char c;
        char* s;
        void* custom;   // generic pointer to a struct or custom data
    } data;
} Value;


#endif //UNIONS_H
