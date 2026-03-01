
bool application_on_event(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_key(const event_type code, u32 sender, u32 listener_inst, event_context context);

bool application_on_resized(const event_type  code, u32 sender, u32 listener_inst, event_context context);

void tetris_dev_set_function_pointers();

bool tetris_game_run();


void main(void)
{
    tetris_dev_set_function_pointers();
    tetris_game_run();
}
