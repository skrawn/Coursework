
#include <stdlib.h>
#include "circ_buf.h"

#ifdef UNIT_TEST
#include <malloc.h>
#include "unit_test.h"
#endif

cb_t *cb_alloc(size_t num_items)
{
	cb_t *cb;
	// Allocate memory from the heap for a cb_t
	cb = malloc(sizeof(cb_t));
	if (cb == NULL)
		return NULL;

	// Allocate memory for the buffer
	cb->buf = malloc(num_items);
	if (cb->buf == NULL) {
		free((void *) cb);
		return NULL;
	}

	cb->head = cb->tail = cb->buf;
	cb->length = num_items;
	cb->num_items = 0;

	return cb;		
} 

void cb_destroy(cb_t *cb)
{
	free((void *) cb->buf);
	free((void *) cb);
}

uint8_t cb_full(cb_t *cb)
{
	return (cb->num_items == cb->length ? cb_status_full : cb_status_ok);
}

uint8_t cb_empty(cb_t *cb)
{
	return (cb->num_items == 0 ? cb_status_empty : cb_status_ok);
}

uint8_t cb_push(cb_t *cb, uint8_t element)
{
	uint8_t retval = cb_status_ok;

	*cb->tail = element;

	if (cb_full(cb) == cb_status_full) {
		retval = cb_status_full;
		cb->num_items = cb->length;
	}
	else
		cb->num_items++;

	// Handle wrap-around
	if (cb->tail == (cb->buf + cb->length - 1)) {
		cb->tail = cb->buf;
	}
	else
		cb->tail++;

	return retval;
}

uint8_t cb_pop(cb_t *cb, uint8_t *element)
{
	uint8_t retval = cb_status_ok;

	if (cb_empty(cb) == cb_status_empty)
		return cb_status_empty;

	*element = *cb->head;
	cb->num_items--;

	// Reset the indices and handle wrap-around
	if (cb->num_items == 0) {
		cb->head = cb->buf;
		cb->tail = cb->head;
		retval = cb_status_empty;
	}
	else if (cb->head == (cb->buf + cb->length - 1)) {
		cb->head = cb->buf;
	}
	else {
		cb->head++;
	}

	return retval;
}

#ifdef UNIT_TEST
void cb_ut_create(void)
{
	ut_suite_t *suite;

	suite = unit_test_create_suite("CB UNIT TEST");
	unit_test_add_test(suite, "BUFFER FULL", cb_ut_buffer_full);
	unit_test_add_test(suite, "BUFFER WRAP", cb_ut_buffer_wrap);
	unit_test_add_test(suite, "BUFFER EMPTY", cb_ut_buffer_empty);
	unit_test_add_test(suite, "BUFFER OVERFLOW", cb_ut_buffer_overflow);
	unit_test_add_test(suite, "BUFFER UNDERFLOW", cb_ut_buffer_underflow);
	unit_test_add_test(suite, "BUFFER CREATE", cb_ut_buffer_create);
	unit_test_add_test(suite, "BUFFER DESTROY", cb_ut_buffer_destroy);	
}

ut_result_t cb_ut_buffer_full(void)
{
	cb_t *cb = cb_alloc(5);
	ut_result_t result;

	// Add 5 items and see if it is now full.
	cb_push(cb, 5);
	cb_push(cb, 5);
	cb_push(cb, 5);
	cb_push(cb, 5);
	cb_push(cb, 5);

	if (cb_full(cb) == cb_status_full)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_wrap(void)
{
	cb_t *cb = cb_alloc(5);
	uint8_t data;
	ut_result_t result;

	// Add 6 items. The 6th item should overwrite the first.
	cb_push(cb, 1);
	cb_push(cb, 2);
	cb_push(cb, 3);
	cb_push(cb, 4);
	cb_push(cb, 5);
	cb_push(cb, 6);

	cb_pop(cb, &data);

	if (data == 6)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_empty(void)
{
	cb_t *cb = cb_alloc(5);
	ut_result_t result;

	// Create an empty circular buffer. Verify it is empty
	if (cb_empty(cb) == cb_status_empty)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_overflow(void)
{
	cb_t *cb = cb_alloc(5);
	ut_result_t result;

	// cb_push should report the buffer is full after 5 items
	cb_push(cb, 1);
	cb_push(cb, 2);
	cb_push(cb, 3);
	cb_push(cb, 4);
	cb_push(cb, 5);

	if (cb_push(cb, 6) == cb_status_full)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_underflow(void)
{
	cb_t *cb = cb_alloc(5);
	uint8_t data;
	ut_result_t result;

	// cb_pop should report the buffer is empty
	if (cb_pop(cb, &data) == cb_status_empty)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_create(void)
{
	cb_t *cb = cb_alloc(5);
	ut_result_t result;

	// cb_alloc should return valid memory
	if (cb != NULL)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	cb_destroy(cb);
	return result;
}

ut_result_t cb_ut_buffer_destroy(void)
{
	cb_t *cb = cb_alloc(5);
	ut_result_t result;

	// Use mallinfo to show that the number of free blocks
	// changes when the circular buffer is freed.
	struct mallinfo info = mallinfo();
	int freeblocks = info.fordblks;

	cb_destroy(cb);

	info = mallinfo();

	if (freeblocks != info.fordblks)
		result = ut_result_pass;
	else
		result = ut_result_fail;

	return result;
}
#endif
