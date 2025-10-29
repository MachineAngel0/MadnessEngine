

#include <stdio.h>

#include "entry_editor.h"
#include "app_types.h"


inline void create_editor_fake(editor_fake* editor_f)
{
    printf("editor created");
    editor_f->update = editor_update;

}

inline void editor_update(editor_fake* editor_f)
{
    printf("editor updating");

}