#include "color_asci.h"

void color_asci_example(void)
{
    printf(ASCII_RED "If" ASCII_BLUE "you" ASCII_YELLOW "are" ASCII_GREEN "bored" ASCII_CYAN "do" ASCII_MAGENTA "this! \n" ASCII_END);
    printf(ASCII_RED_BOLD "If" ASCII_BLUE_BOLD "you" ASCII_YELLOW_BOLD "are" ASCII_GREEN "bored" ASCII_CYAN "do" ASCII_MAGENTA "this! \n" ASCII_END);
}

