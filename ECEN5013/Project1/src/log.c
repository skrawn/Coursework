
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "log.h"

#ifdef FRDM
#include "uart.h"
#endif

#define NEWLINE_BYTE			1
#define LINUX_BUFFER_SIZE		1024
#define PARAM_BUFFER_SIZE		32

void log_0(uint8_t *data, size_t len)
{
#ifdef FRDM
	uint8_t buffer[TX_BUFFER_LENGTH] = {0};
	if ((len + NEWLINE_BYTE) > TX_BUFFER_LENGTH)
		// Have to truncate
		len = TX_BUFFER_LENGTH;

	memcpy(buffer, data, len);
	*(buffer + len) = '\n';
	uart_put_tx_buf(buffer, len + NEWLINE_BYTE);
#else
	printf("%s\n", data);
#endif
}


void log_1(uint8_t *data, size_t len, void *param, log_data_t data_type_size)
{
	uint8_t param_buffer[PARAM_BUFFER_SIZE] = {0};
	uint8_t param_len;

	// Convert the input parameter based on the data type
	switch (data_type_size) {
		case log_float_t:
			ftoa(*((float *) param), param_buffer);
			break;

		case log_uint8_t:
			my_itoa(param_buffer, (int32_t) *((uint8_t *) param), 10);
			break;

		case log_int8_t:
			my_itoa(param_buffer, (int32_t) *((int8_t *) param), 10);
			break;

		case log_uint16_t:
			my_itoa(param_buffer, (int32_t) *((uint16_t *) param), 10);
			break;

		case log_int16_t:
			my_itoa(param_buffer, (int32_t) *((int16_t *) param), 10);
			break;

		case log_uint32_t:
			my_itoa(param_buffer, (int32_t) *((uint32_t *) param), 10);
			break;

		case log_int32_t:
			my_itoa(param_buffer, (int32_t) *((int32_t *) param), 10);
			break;		

		default:
			// Assume everything else is a char
			*param_buffer = *((char *) param);
			break;
	}


	param_len = (uint8_t) strlen((const char *) param_buffer);

#ifdef FRDM
	uint8_t buffer[TX_BUFFER_LENGTH] = {0};

	if ((len + param_len + NEWLINE_BYTE) > TX_BUFFER_LENGTH) {
		// Have to truncate, do it on the message part, not the param
		len = TX_BUFFER_LENGTH - param_len;
	}

	memcpy(buffer, data, len);
	memcpy(buffer + len, param_buffer, param_len);
	*(buffer + len + param_len) = '\n';
	uart_put_tx_buf(buffer, len + param_len + NEWLINE_BYTE);
#else
	// This section would be run on the BBB or on an x86_64 machine, so
	// I can allocate a kilobyte for this
	uint8_t buffer[LINUX_BUFFER_SIZE] = {0};
	if ((len + param_len) > LINUX_BUFFER_SIZE) {
		// Truncate the message part
		len = LINUX_BUFFER_SIZE - param_len;
	}

	memcpy(buffer, data, len);
	// Printf doesn't expect the extra null that gets passed in
	// with data. Just knock off an extra byte.
	memcpy(buffer + len - 1, param_buffer, param_len);
	printf("%s\n", buffer);
#endif
}

