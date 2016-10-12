
#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <stdint.h>

/**
 * @brief Initializes the profiler functions
 */ 
void profiler_init(void);

/**
 * @brief Returns the current time in microseconds
 * @return Returns the the current time in microseconds.
 */ 
uint32_t profiler_get_time(void);

/**
 * @brief Calculates the difference between two time stamps.
 * @param start Start time.
 * @param end End time.
 * @return Returns the the current time in microseconds.
 */ 
uint32_t profiler_get_time_diff(uint32_t start, uint32_t end);

#endif
