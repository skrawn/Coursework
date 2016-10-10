
#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <stdint.h>

void profiler_init(void);
uint32_t profiler_get_time(void);
uint32_t profiler_get_time_diff(uint32_t start, uint32_t end);

#endif