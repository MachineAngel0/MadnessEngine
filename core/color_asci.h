

#ifndef COLOR_ASCI_H
#define COLOR_ASCI_H


//shout out to this repository
// https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a

// and this stack overflow
//https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences


//Color
#define ASCII_BLACK "\033[0;30m"
#define ASCII_RED "\033[0;31m"
#define ASCII_GREEN "\033[0;32m"
#define ASCII_YELLOW "\033[0;33m"
#define ASCII_BLUE "\033[0;34m"
#define ASCII_MAGENTA "\033[0;35m"
#define ASCII_CYAN "\033[0;36m"
#define ASCII_GREY "\033[0;37m"

//Bold

#define ASCII_BOLD "\033[1m" // WHITE


#define ASCII_BLACK_BOLD "\033[1;30m"
#define ASCII_RED_BOLD "\033[1;31m"
#define ASCII_GREEN_BOLD "\033[1;32m"
#define ASCII_YELLOW_BOLD "\033[1;33m"
#define ASCII_BLUE_BOLD "\033[1;34m"
#define ASCII_MAGENTA_BOLD "\033[1;35m"
#define ASCII_CYAN_BOLD "\033[1;36m"

/* TODO: not in use yet, and im too lazy to recolor it
//Underline
#define UBLK "\033[4;30m"
#define URED "\033[4;31m"
#define UGRN "\033[4;32m"
#define UYEL "\033[4;33m"
#define UBLU "\033[4;34m"
#define UMAG "\033[4;35m"
#define UCYN "\033[4;36m"
#define UWHT "\033[4;37m"

//Background
#define BLKB "\033[40m"
#define REDB "\033[41m"
#define GRNB "\033[42m"
#define YELB "\033[43m"
#define BLUB "\033[44m"
#define MAGB "\033[45m"
#define CYNB "\033[46m"
#define WHTB "\033[47m"
*/

#define ASCII_END "\033[0m"


void color_asci_example(void)
{
    printf(ASCII_RED "If" ASCII_BLUE "you" ASCII_YELLOW "are" ASCII_GREEN "bored" ASCII_CYAN "do" ASCII_MAGENTA "this! \n" ASCII_END);
    printf(ASCII_RED_BOLD "If" ASCII_BLUE_BOLD "you" ASCII_YELLOW_BOLD "are" ASCII_GREEN "bored" ASCII_CYAN "do" ASCII_MAGENTA "this! \n" ASCII_END);
}


#endif //COLOR_ASCI_H
