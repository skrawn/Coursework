

#ifndef _CIRCBUF_H
#define _CIRCBUF_H

#include <stdint.h>

typedef struct {
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

uint8_t cb_full(cb_t *cb);
uint8_t cb_empty(cb_t *cb);
uint8_t cb_push(cb_t *cb, uint8_t element);
uint8_t cb_empty(cb_t *cb, uint8_t *element);

#endif
