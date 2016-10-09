
#include <stdint.h>

typedef enum {
	log_uint8_t,
	log_uint16_t,
	log_uint32_t,
	log_int8_t,
	log_int16_t,
	log_int32_t,
	log_char_t,
	log_float_t
} log_data_t;

void log_0(uint8_t *data, size_t len);
void log_1(uint8_t *data, size_t len, void *param, log_data_t data_type_size);
