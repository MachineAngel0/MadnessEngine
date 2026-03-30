
#ifndef ENTRY_EDITOR_H
#define ENTRY_EDITOR_H

#include "app_types.h"

// Externally-defined function to create a game. make sure that the c compiler finds this function somewhere
extern void editor_create_fpn(Editor_Application* editor_app);

int main()
{

    Editor_Application editor_app;
    editor_create_fpn(&editor_app);
    editor_app_run(&editor_app);


    return 0;
}

#endif
