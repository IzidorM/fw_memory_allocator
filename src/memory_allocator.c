/*
 * SPDX-FileCopyrightText: 2024 Izidor Makuc <izidor@makuc.info>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "memory_allocator_internal.h"

#ifdef UNIT_TESTS
#define STATIC
#else
#define STATIC static
#endif

// not used anymore, but left here for potential future use
//STATIC uint8_t *addr_allign_down(uint8_t *mem_start, 
//				     intptr_t align)
//{
//	intptr_t addr  = (intptr_t) mem_start;
//	addr &= -align;   // Round up to align-byte boundary
//	return (uint8_t *) addr;
//}

STATIC uint8_t *addr_allign_up(uint8_t *mem_start, 
				   intptr_t align)
{
	intptr_t addr  = (intptr_t) mem_start;
	addr = (addr + (align - 1)) & -align;
	return (uint8_t *) addr;
}

STATIC bool check_if_enought_mem_is_available(uint8_t *start,
					      uint8_t *end,
					      const size_t size)
{
	if (start > end)
	{
		return false;
	}
	else
	{
		return (size_t) (end - start) >= size;
	}
}

STATIC void write_meta_data(struct mem_meta_data *m, 
			    uint8_t *mem_start, 
			    size_t mem_size)
{
	m->start_of_mem_segment = mem_start;
	m->mem_segment_size = mem_size;
}

STATIC void set_overflow_detection_buff(
	uint8_t *start_of_overflow_detection_buff)

{
	const uint8_t overflow_detection_buff_size = 
		(int8_t) OVERFLOW_DETECTION_BUFF_SIZE;

	// fill the overflow detection buffer with decreasing values 
	// ending with 0. 
	for (uint8_t i = 0; 
	     overflow_detection_buff_size > i ; i++)
	{
		start_of_overflow_detection_buff[i] = 
			overflow_detection_buff_size - (uint8_t)(i+1);
	}
}

STATIC bool check_if_overflow_detection_buff_is_valid(
	uint8_t *start_of_overflow_detection_buff)
{
	const uint8_t overflow_detection_buff_size = 
		(int8_t) OVERFLOW_DETECTION_BUFF_SIZE;

	for (uint8_t i = 0; 
	     overflow_detection_buff_size > i ; i++)
	{
		if (start_of_overflow_detection_buff[i] != 
		    overflow_detection_buff_size - (uint8_t)(i+1))
		{
			return false;
		}
	}

	return true;
}

// get a chunk of non-freeable memory
void *mem_allocator_malloc_static(struct mem_allocator *m, const size_t size)
{
        if(NULL == m || true == m->mem_lock || 0 == size)
	{
		return NULL;
	}
        
	// allign to the architecture default allignment
        m->start_of_free_mem = addr_allign_up(m->start_of_free_mem,
						  sizeof(void *));

	size_t size_needed_with_overflow_detection_buffer =
		size + OVERFLOW_DETECTION_BUFF_SIZE;

        if (check_if_enought_mem_is_available(
		    m->start_of_free_mem,
		    (uint8_t *) m->end_of_meta_data,
		    size_needed_with_overflow_detection_buffer
		    + sizeof(struct mem_meta_data)))
        {
		uint8_t *start_of_overflow_detection_buff = 
			m->start_of_free_mem + size;

		set_overflow_detection_buff(
			start_of_overflow_detection_buff);


		// write meta data
		m->end_of_meta_data -= 1;
		write_meta_data(m->end_of_meta_data, 
				m->start_of_free_mem, size);


                uint8_t *r = m->start_of_free_mem;
                m->start_of_free_mem += 
			size_needed_with_overflow_detection_buffer;
		
		return r;
	}

        return NULL;
}

struct mem_allocator *mem_allocator_init(uint8_t *memory, 
					 size_t mem_size)
{
	if (NULL == memory || mem_size <= sizeof(struct mem_allocator))
	{
		return NULL;
	}

	uint8_t *memory_alligned = addr_allign_up(memory,
						      sizeof(void *));

	if (memory != memory_alligned)
	{
		mem_size -= sizeof(void *);
	}

	// fake mem_allocator to use mem_malloc_static
	// to allocate the memory for the real mem_allocator
	// on memory provided by user
	struct mem_allocator tmp = {
		.memory = memory_alligned,
		.start_of_free_mem = memory_alligned,
		.end_of_meta_data = (struct mem_meta_data *)
		((uint8_t *) memory_alligned 
		 + (mem_size & -sizeof(void *))),
		.memory_size = mem_size,
		.mem_lock = false,
	};
	
	struct mem_allocator *m= mem_allocator_malloc_static(
		&tmp, 
		sizeof(struct mem_allocator));

	if (m)
	{
		for (uint32_t i = 0; 
		     sizeof(struct mem_allocator) > i; i++)
		{
			((uint8_t *) m)[i] = ((uint8_t *) &tmp)[i];
		}
	}

	return m;
}

// Locking memory
// In embedded systems all memory should be allocated at startup
// So after startup mem lock should be set, so all calls to malloc
// will fail and notify the user about unexpected behaviour
void mem_allocator_set_lock(struct mem_allocator *m)
{
	//TODO: Handle NULL pointer
        m->mem_lock = true;
}

bool mem_allocator_check_for_overflow(struct mem_allocator *m)
{
	//TODO: Handle NULL pointer
	for (struct mem_meta_data *meta = m->end_of_meta_data ; 
	     (m->memory + m->memory_size) > (uint8_t *) meta; 
	     meta++)
	{
		uint8_t *start_of_overflow_detection_buff = 
			meta->start_of_mem_segment 
			+ meta->mem_segment_size;

		bool tmp = check_if_overflow_detection_buff_is_valid(
			start_of_overflow_detection_buff);

		if (false == tmp)
		{
			return true;
		}
	}
	return false;
}

