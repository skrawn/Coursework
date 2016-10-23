/**
 * @file unit_test.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Unit testing framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "circ_buf.h"
#include "log.h"
#include "uart.h"
#include "unit_test.h"

#define MAX_NAME_LEN	20
#define NULL_CHAR_LEN	1

static void unit_test_run_test(ut_suite_t *suite, ut_test_t *test);
static void unit_test_destroy_suite(ut_suite_t *ut_suite);

ut_suite_t *ut_suites = NULL;

void unit_test_run() 
{
	ut_suite_t *active_suite, *last_suite;
	ut_test_t *active_test;
	uint32_t pass_count;
	uint8_t result_size;
	char result_buf[100] = {0};

	active_suite = ut_suites;

	while (active_suite != NULL)
	{		
		memset(result_buf, 0, sizeof(result_buf));
		active_test = active_suite->test_list;
		pass_count = 0;
		while (active_test != NULL) {
			unit_test_run_test(active_suite, active_test);
			if (active_test->result == ut_result_pass)
				pass_count++;
			active_test = active_test->next_test;
		}

		if (pass_count == active_suite->num_tests)
			result_size = sprintf(result_buf, "%s SUITE: PASS", active_suite->suite_name);
		else
			result_size = sprintf(result_buf, "%s SUITE: FAIL (%u/%u PASS)", active_suite->suite_name,
					pass_count, active_suite->num_tests);

		log_0((uint8_t *) result_buf, result_size);
		uart_wait_tx_buf_empty();
		last_suite = active_suite;
		active_suite = active_suite->next_suite;
		unit_test_destroy_suite(last_suite);
	}

	log_0((uint8_t *) "\nUNIT TESTS COMPLETE", sizeof("\nUNIT TESTS COMPLETE"));
	uart_wait_tx_buf_empty();
}

void unit_test_add_test(ut_suite_t *ut_suite, char *test_name, ut_result_t (*test)(void))
{
	ut_test_t *new_test, *test_list;

	// Create the first test in the list if it does not exist
	if (ut_suite->test_list == NULL) {
		new_test = malloc(sizeof(ut_test_t));
		ut_suite->test_list = new_test;
		new_test->test_number = 1;
	}
	else {
		// Add a new test to the list
		test_list = ut_suite->test_list;
		while (test_list->next_test != NULL) {
			test_list = test_list->next_test;
		}
		new_test = malloc(sizeof(ut_test_t));
		test_list->next_test = new_test;
		new_test->test_number = test_list->test_number + 1;
	}
	new_test->next_test = NULL;
	strncpy(new_test->test_name, test_name, MAX_NAME_LEN);
	new_test->test = test;	
	new_test->result = ut_result_notrun;

	ut_suite->num_tests++;		
}

ut_suite_t *unit_test_create_suite(char *suite_name)
{
	ut_suite_t *new_suite, *suite;

	// Check if a test suite already exists
	if (ut_suites == NULL) {
		new_suite = malloc(sizeof(ut_suite_t));
		ut_suites = new_suite;		
	}
	else {
		// Otherwise, add a new suite to the list
		suite = ut_suites->next_suite;
		while(suite != NULL) {
			suite = suite->next_suite;
		}
		new_suite = malloc(sizeof(ut_suite_t));
		suite = new_suite;		
	}
	new_suite->next_suite = NULL;
	strncpy(new_suite->suite_name, suite_name, MAX_NAME_LEN);
	new_suite->num_tests = 0;
	new_suite->test_list = NULL;

	return new_suite;
}

static void unit_test_run_test(ut_suite_t *suite, ut_test_t *test)
{
	char result_buf[100] = {0};
	uint32_t result_length;

	ut_result_t result = test->test();
	test->result = result;
	
	if (result == ut_result_pass) {
		result_length = sprintf(result_buf, "%s: %u/%u - %s ... PASS", suite->suite_name,
			test->test_number, suite->num_tests, test->test_name);
	}
	else {
		result_length = sprintf(result_buf, "%s: %u/%u - %s ... FAIL", suite->suite_name,
			test->test_number, suite->num_tests, test->test_name);
	}
	log_0((uint8_t *) result_buf, result_length);
}

static void unit_test_destroy_suite(ut_suite_t *ut_suite)
{
	ut_test_t *test, *delete_test;

	// Delete the tests associated with this suite
	test = ut_suite->test_list;
	while (test != NULL) {
		delete_test = test;
		test = test->next_test;
		free(delete_test);
	}

	// Delete the suite
	free(ut_suite);
}
