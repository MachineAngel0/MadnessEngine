
#pragma once

#include "app_types.h"

// Externally-defined function to create a game. make sure that the c compiler finds this function somewhere
extern void create_editor_fake(struct editor_fake* editor_f);
void editor_update(struct editor_fake* editor_f);




int main()
{
    editor_fake editor_f;
    create_editor_fake(&editor_f);
    editor_update(&editor_f);
}
