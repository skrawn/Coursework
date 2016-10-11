
#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

#include <stdint.h>

#define MAX_NAME_LEN	20
#define NULL_CHAR_LEN	1

typedef enum {
	ut_result_fail = 0,
	ut_result_pass,
	ut_result_notrun
} ut_result_t;

typedef struct ut_test_t ut_test_t;

struct ut_test_t {
	char test_name[MAX_NAME_LEN + NULL_CHAR_LEN];
	uint32_t test_number;
	ut_result_t result;
	ut_test_t *next_test;
	ut_result_t (*test)(void);
};

typedef struct ut_suite_t ut_suite_t;

struct ut_suite_t {
	char suite_name[MAX_NAME_LEN + NULL_CHAR_LEN];
	uint32_t num_tests;	
	ut_test_t *test_list;
	ut_suite_t *next_suite;
};

void unit_test_run();
void unit_test_add_test(ut_suite_t *ut_suite, char *test_name, ut_result_t (*test)(void));
ut_suite_t *unit_test_create_suite(char *suite_name);


#endif
