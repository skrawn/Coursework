
#ifndef _CIRC_BUF_H_
#define _CIRC_BUF_H_

#include <stddef.h>
#include <stdint.h>

typedef volatile struct {
	uint8_t *head;
	uint8_t *tail;
	uint8_t *buf;
	size_t length;
	size_t num_items;
} cb_t;

typedef enum {
	cb_status_empty,
	cb_status_ok,
	cb_status_full
} cb_status_t;

cb_t *cb_alloc(size_t num_items);
void cb_destroy(cb_t *cb); 
uint8_t cb_full(cb_t *cb);
uint8_t cb_empty(cb_t *cb);
uint8_t cb_push(cb_t *cb, uint8_t element);
uint8_t cb_pop(cb_t *cb, uint8_t *element);

#ifdef UNIT_TEST
#include "unit_test.h"
void cb_ut_create(void);
ut_result_t cb_ut_buffer_full(void);
ut_result_t cb_ut_buffer_wrap(void);
ut_result_t cb_ut_buffer_empty(void);
ut_result_t cb_ut_buffer_overflow(void);
ut_result_t cb_ut_buffer_underflow(void);
ut_result_t cb_ut_buffer_create(void);
ut_result_t cb_ut_buffer_destroy(void);
#endif

#endif
