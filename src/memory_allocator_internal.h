/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MEMORY_ALLOCATOR_INTERNAL_H
#define MEMORY_ALLOCATOR_INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define OVERFLOW_DETECTION_BUFF_SIZE sizeof(void *)

struct mem_meta_data {
	uint8_t *start_of_mem_segment;
	size_t mem_segment_size;
};

struct mem_allocator {
	uint8_t *memory;
	uint8_t *start_of_free_mem;
	struct mem_meta_data *end_of_meta_data;
	size_t memory_size;
	bool mem_lock;
};

#ifdef UNIT_TESTS
uint8_t *addr_allign_up(uint8_t *mem_start, intptr_t align);
uint8_t *addr_allign_down(uint8_t *mem_start, intptr_t align);
bool check_if_enought_mem_is_available(uint8_t *start,
				       uint8_t *end,
				       const size_t size);

void write_meta_data(struct mem_meta_data *m, 
		     uint8_t *mem_start, 
		     size_t mem_size);

void set_overflow_detection_buff(
	uint8_t *start_of_overflow_detection_buff);

bool check_if_overflow_detection_buff_is_valid(
	uint8_t *start_of_overflow_detection_buff);

#endif

#endif
