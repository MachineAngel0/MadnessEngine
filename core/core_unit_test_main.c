
int main(void)
{
    memory_tracker_init();
    u64 memory_request_size = MB(1);
    memory_system_init(memory_request_size);


    arena_test();
    arena_stack_test();


    _darray_test();
    array_test();

    //TODO: units tests and finish up the dsa's remaining
    // arena_test();
    // arena_freelist_test();
    // arena_pool_test();
    // array_test();

    binary_tree_test();
    // b_tree_red_black_test();
    hash_map_test();
    hash_map_string_test();
    hash_set_test();
    // heap_test();
    linked_list_test();
    queue_test();
    stack_test();
    string_test();
    string_builder_test();

    memory_tracker_print_memory_usage();


    TEST_REPORT_TOTAL();
}
