
#include "unit_test.h"

void test_expr(const bool val, const char* msg, const char* file, const int line,
               const bool inform_passing, const bool inform_failing)
{

    if (val)
    {
        if (inform_passing)
        {
            printf(ASCII_GREEN_BOLD"PASSING: "ASCII_END "%s, %s, %d\n", msg, file, line);
        }
        passing += 1;
    }
    else
    {
        if (inform_failing)
        {
            printf(ASCII_RED_BOLD"FAILING: "ASCII_END "%s, %s, %d\n", msg, file, line);
        }
        failing += 1;
    }
}


void test_print_normal(void)
{
    printf("PASSING: %d, FAILING: %d\n", passing, failing);
}

void test_start(const char* test_name)
{
    DEBUG("%s START", test_name);

}

void test_report_print(const char* test_name)
{
    printf(ASCII_BOLD"%s: "ASCII_END, test_name);
    printf(
        ASCII_BOLD "PASSING: " ASCII_GREEN_BOLD "%d, "ASCII_END ASCII_BOLD"FAILING: " ASCII_RED_BOLD "%d" ASCII_END
        "\n", passing, failing);

    //
    total_passing += passing;
    total_failing += failing;
    passing = 0;
    failing = 0;

    DEBUG("%s END\n", test_name);

}

void test_report_final_total(void)
{
    printf("\n"ASCII_MAGENTA_BOLD "FINAL: " ASCII_END
        ASCII_BOLD "TOTAL PASSING: " ASCII_GREEN_BOLD "%d, "ASCII_END ASCII_BOLD "TOTAL FAILING: " ASCII_RED_BOLD "%d" ASCII_END
        "\n", total_passing, total_failing);
}




