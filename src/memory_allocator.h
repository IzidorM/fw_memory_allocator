/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MEMORY_INTERNAL_H
#define MEMORY_INTERNAL_H

#include <stdint.h>
#include <stdlib.h>

struct mem_allocator;

void mem_allocator_set_lock(struct mem_allocator *m);

void *mem_allocator_malloc_static(struct mem_allocator *m, const size_t size);

bool mem_allocator_check_for_overflow(struct mem_allocator *m);

struct mem_allocator *mem_allocator_init(uint8_t *memory, 
					 size_t mem_size);


#endif
