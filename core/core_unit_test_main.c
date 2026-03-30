
int main(void)
{
    u64 memory_request_size = MB(1);
    Memory_System memory_system;
    memory_system_init(&memory_system, memory_request_size, TODO);



    arena_test();
    arena_stack_test();
    free_list_test();

    array_macro_test();

    array_test();
    _darray_test();

    //TODO: units tests and finish up the dsa's remaining
    // arena_test();
    // arena_freelist_test();
    // arena_pool_test();
    // array_test();

    binary_tree_test();
    // b_tree_red_black_test();
    hash_map_test();
    hash_table_test();
    hash_set_test();
    // heap_test();
    linked_list_test();
    queue_test();
    stack_test();
    string_test();
    string_builder_test();



    memory_system_shutdown(&memory_system);


    TEST_REPORT_TOTAL();
}
