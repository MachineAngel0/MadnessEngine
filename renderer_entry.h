#ifndef ENTRY_RENDERER_H
#define ENTRY_RENDERER_H

#include "application.h"
#include "renderer.h"


extern void create_renderer(struct renderer* renderer_out);



int main(void)
{
    struct renderer renderer;
    create_renderer(&renderer);

    application_renderer_create(&renderer);

    return 0;
}


#endif