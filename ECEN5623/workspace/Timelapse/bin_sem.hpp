/******************************************************************************
 * @file bin_sem.c
 * @brief Binary semaphore
 * @author Sean Donohue
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Sean Donohue</b>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Some portions of this code belong to this Stack Overflow thread:
 * http://stackoverflow.com/questions/7478684/how-to-initialise-a-binary-semaphore-in-c
 *
 *
 *******************************************************************************/

#ifndef BIN_SEM_H_
#define BIN_SEM_H_

#include <pthread.h>

struct binary_semaphore {
	pthread_mutex_t mutex;
	pthread_cond_t cvar;
	int v;
};

void bin_sem_init(struct binary_semaphore *p, int init_val);
void bin_sem_post(struct binary_semaphore *p);
void bin_sem_wait(struct binary_semaphore *p);
void bin_sem_destroy(struct binary_semaphore *p);

#endif /* BIN_SEM_H_ */
