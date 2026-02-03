#ifndef ENTRY_RENDERER_H
#define ENTRY_RENDERER_H



extern void create_renderer(struct renderer_app* renderer_out);

int main(void)
{
    struct renderer_app renderer;
    create_renderer(&renderer);

    application_renderer_create(&renderer);

    return 0;
}


#endif