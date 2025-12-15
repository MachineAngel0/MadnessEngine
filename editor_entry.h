
#ifndef ENTRY_EDITOR_H
#define ENTRY_EDITOR_H

#include "application.h"
#include "app_types.h"

// Externally-defined function to create a game. make sure that the c compiler finds this function somewhere
extern void editor_create(struct editor_app* editor_f);




int main()
{
    editor_app editor_f;
    editor_create(&editor_f);

    application_editor_create(&editor_f);

    return 0;
}

#endif
