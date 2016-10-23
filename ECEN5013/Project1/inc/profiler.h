/**
 * @file profiler.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Code profiling functions
 */

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

/**
 * @brief Wrapper function to start the profiler.
 */
void profiler_start(void);

/**
 * @brief Wrapper function to stop the profiler.
 */
void profiler_stop(void);

/**
 * @brief Wrapper function to find the profiler result.
 * @return Returns the run-time in microseconds.
 */
uint32_t profiler_get_result(void);

#endif
