

#include <stdio.h>

#include "editor_entry.h"
#include "app_types.h"


inline void editor_create(editor* editor_f)
{
    printf("editor created");
    editor_f->update = editor_update;

}

inline void editor_update(editor* editor_f)
{
    printf("editor updating");

}