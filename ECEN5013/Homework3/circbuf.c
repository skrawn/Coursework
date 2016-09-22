
#include <stdint.h>

#include "circbuf.h"

uint8_t cb_full(cb_t *cb) 
{
	return (cb->num_items >= cb->length ? cb_status_full : cb_status_ok);
}

uint8_t cb_empty(cb_t *cb)
{
	return (cb->num_items == 0 ? cb_status_empty : cb_status_ok);
}

uint8_t cb_push(cb_t *cb, uint8_t element)
{
	uint8_t retval = cb_status_ok;

	*cb->tail = element;
	cb->num_items++; 
	
	if (cb_full(cb) == cb_status_full) {
		retval = cb_status_full;
		cb->num_items = cb->length;
	}

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


