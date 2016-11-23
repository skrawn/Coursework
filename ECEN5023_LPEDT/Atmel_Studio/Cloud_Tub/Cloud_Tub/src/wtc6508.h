/*
 * wtc6508.h
 *
 * Created: 11/19/2016 3:13:00 PM
 *  Author: Sean
 */ 

#include <stdint.h>

#include "status_codes.h"

#ifndef WTC6508_H_
#define WTC6508_H_

void wtc6508_init(void);
enum status_code wtc6508_read(uint8_t *status);

#endif /* WTC6508_H_ */