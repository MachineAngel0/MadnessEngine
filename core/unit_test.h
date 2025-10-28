#ifndef UNIT_TEST_H
#define UNIT_TEST_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "color_asci.h"


static uint32_t passing = 0;
static uint32_t failing = 0;
static uint32_t prev_passing = 0;
static uint32_t prev_failing = 0;

//NOTE: might want another function do a uint32 as a bool instead, checking 0 for fail, and anything else as success
void test(bool val)
{
    if (val)
    {
        passing += 1;
    }
    else
    {
        failing += 1;
    }
}

void test_expr(bool val, const char* msg, const char* file, int line,
               bool inform_passing, bool inform_failing)
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

void test_print_normal()
{
    printf("PASSING: %d, FAILING: %d\n", passing, failing);
}


void test_report_print(const char* test_name)
{
    printf(ASCII_BOLD"%s: "ASCII_END, test_name);
    printf(
        ASCII_BOLD "PASSING: " ASCII_GREEN_BOLD "%d, "ASCII_END ASCII_BOLD"FAILING: " ASCII_RED_BOLD "%d" ASCII_END
        "\n", passing-prev_passing, failing-prev_failing);

    //
    prev_passing = passing;
    prev_failing = failing;
}


//TODO: go through the platform print colors
void test_print_platform()
{
    return;
}

//macro for having it look nice
//takes in a bool, and gives back no information
#define TEST(func) test(func)

//prints where the file is failing, if it is
#define TEST_DEBUG(func) test_expr(func, #func, __FILE__, __LINE__, false, true)
//prints where the file is passing and failing
#define TEST_INFORM(func) test_expr(func, #func, __FILE__, __LINE__, true, true)

//prints where the file is failing, and the expression that it is checking against like (a == 10 )
#define TEST_EXPR(func, expr) test_expr(func, #expr, __FILE__, __LINE__. false, true)

//pass in a string
//returns the report for your specific test, not for all tests
#define TEST_REPORT(test_name) test_report_print(#test_name)


#endif //UNIT_TEST_H
