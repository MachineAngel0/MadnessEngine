#ifndef UNIT_TEST_H
#define UNIT_TEST_H


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "logger.h"
#include "color_asci.h"


static uint32_t passing = 0;
static uint32_t failing = 0;
static uint32_t total_passing = 0;
static uint32_t total_failing = 0;


void test_expr(const bool val, const char* msg, const char* file, const int line,
               const bool inform_passing, const bool inform_failing);

void test_print_normal(void);

void test_start(const char* test_name);

void test_report_print(const char* test_name);

void test_report_final_total(void);




//macro for having it look nice
//performs the test but gives no debug info, if something is a success or a fail
#define TEST(func) test_expr(func, #func, __FILE__, __LINE__, false, false)

//prints where the file is failing, if it is
#define TEST_DEBUG(func) test_expr(func, #func, __FILE__, __LINE__, false, true)
//prints where the file is passing and failing
#define TEST_INFORM(func) test_expr(func, #func, __FILE__, __LINE__, true, true)

//prints where the file is failing, and the expression that it is checking against like (a == 10 )
#define TEST_EXPR(func, expr) test_expr(func, #expr, __FILE__, __LINE__. false, true)

//JUST A NICE DEBUG THING
#define TEST_START(test_name) test_start(#test_name)
//pass in a string
//returns the report for your specific test, not for all tests
#define TEST_REPORT(test_name) test_report_print(#test_name)
//literally just the same thing as TEST_REPORT
#define TEST_END(test_name) test_report_print(#test_name)


//reports the accumulative passing and failing scores
#define TEST_REPORT_TOTAL() test_report_final_total()


#endif //UNIT_TEST_H
