
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

inline uint8_t cb_full(cb_t *cb);
inline uint8_t cb_empty(cb_t *cb);
inline uint8_t cb_push(cb_t *cb, uint8_t element);
inline uint8_t cb_pop(cb_t *cb, uint8_t *element);


#endif
