#include "core/platform/event.h"


//sets up function pointers
extern void renderer_dev_create();


bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type  code, u32 sender, u32 listener_inst, event_context context);


bool renderer_dev_run();


void main(void)
{

    renderer_dev_create();
    renderer_dev_run();
}
