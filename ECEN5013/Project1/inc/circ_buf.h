
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

/**
 * @brief Allocates a circular buffer on the heap.
 * @param num_items Number of bytes to allocate for the buffer/
 * @return Pointer to circular buffer in dynamic memory.
 */
cb_t *cb_alloc(size_t num_items);

/**
 * @brief Frees a circular buffer from the heap.
 * @param cb Pointer to the circular buffer to free.
 */
void cb_destroy(cb_t *cb); 

/**
 * @brief Checks if the circular buffer is full.
 * @param cb Pointer to the circular buffer.
 * @return Returns cb_status_full if full.
 */ 
uint8_t cb_full(cb_t *cb);

/**
 * @brief Checks if the circular buffer is empty.
 * @param cb Pointer to the circular buffer.
 * @return Returns cb_status_empty if empty.
 */ 
uint8_t cb_empty(cb_t *cb);

/**
 * @brief Puts an element into the circular buffer
 * @param cb Pointer to the circular buffer.
 * @param element Element to put in the circular buffer.
 * @return Returns cb_status_ok if the item was queued and the
 * buffer is not full.
 */ 
uint8_t cb_push(cb_t *cb, uint8_t element);

/**
 * @brief Pulls an element out of the circular buffer
 * @param cb Pointer to the circular buffer.
 * @param element Pointer to the byte to store the returned data.
 * @return Returns cb_status_ok if the item was returned and 
 * buffer is not empty.
 */ 
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
