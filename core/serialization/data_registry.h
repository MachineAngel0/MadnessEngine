#ifndef DATA_REGISTRY_H
#define DATA_REGISTRY_H
#include <stdbool.h>


//?? this should all just be part of the reflection system now that im looking at it
typedef struct Data_Field
{
    char* field_name;
    enum data_type type;
    void* data;
} Data_Field;

typedef struct Data_Object
{
    char* name;
    Data_Field* head;
    Data_Field* last;
} Data_Object;

typedef struct Data_Registry
{
    bool unimplemented;
} Data_Registry;


#endif //DATA_REGISTRY_H
