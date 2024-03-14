#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "unity.h"

#include "memory_allocator_internal.h"
#include "memory_allocator.h"

void setUp(void)
{

}

void tearDown(void)
{
        
}

static struct mem_allocator *helper_get_mem_alloc_struct(
	uint8_t *mem, size_t mem_size)
{
	struct mem_allocator *m = malloc(sizeof(struct mem_allocator));

	if (m == NULL) {
		return NULL;
	}

	m->memory = mem;
	m->start_of_free_mem = mem;
	m->end_of_meta_data = (struct mem_meta_data *)
		((uint8_t *) mem + mem_size);
	m->memory_size = mem_size;
	m->mem_lock = false;
	
	return m;
}

// not used atm, but left here for potential future use
//void test_allign_down(void)
//{
//	// T1
//	uint8_t *test_ptr = (uint8_t *) (sizeof(void *) - 1);
//
//	TEST_ASSERT_EQUAL_PTR(NULL, 
//			      addr_allign_down(
//				      test_ptr, sizeof(void *)));
//
//	// T2
//	test_ptr = (uint8_t *) (sizeof(uint32_t) - 1);
//
//	TEST_ASSERT_EQUAL_PTR(NULL, 
//			      addr_allign_down(
//				      test_ptr, sizeof(uint32_t)));
//	
//	// T3
//
//	test_ptr = (uint8_t *) ((2 * sizeof(uint16_t)) - 1);
//
//	TEST_ASSERT_EQUAL_PTR(sizeof(uint16_t), 
//			      addr_allign_down(
//				      test_ptr, sizeof(uint16_t)));
//
//}

void test_mem_allign_up(void)
{
	// T1- test alligning a machine default ptr
	uint8_t *test_ptr = (uint8_t *) (sizeof(void *) - 1);

	TEST_ASSERT_EQUAL_PTR((uint8_t *)sizeof(void *), 
			      addr_allign_up(
				      test_ptr, sizeof(void *)));

	// T2- test alligning a uint32_t ptr
	test_ptr = (uint8_t *) (sizeof(uint32_t) - 1);

	TEST_ASSERT_EQUAL_PTR((uint8_t *) sizeof(uint32_t), 
			      addr_allign_up(
				      test_ptr, sizeof(uint32_t)));
	
	// T3- test alligning a uint16_t ptr
	test_ptr = (uint8_t *) ((2 * sizeof(uint16_t)) - 1);

	TEST_ASSERT_EQUAL_PTR(2 * sizeof(uint16_t), 
			      addr_allign_up(
				      test_ptr, sizeof(uint16_t)));
}


void test_check_if_enought_mem_is_available(void)
{
	uint8_t *start_of_mem_segment = (uint8_t *) 0x0;
	uint8_t *end_of_mem_segment = (uint8_t *) 0x0;
	TEST_ASSERT_TRUE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			0)
		);

	start_of_mem_segment = (uint8_t *) 0x0;
	end_of_mem_segment = (uint8_t *) 0x8;
	TEST_ASSERT_TRUE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			5)
		);


	start_of_mem_segment = (uint8_t *) 0x5;
	end_of_mem_segment = (uint8_t *) 0x23;
	TEST_ASSERT_TRUE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			7)
		);


	start_of_mem_segment = (uint8_t *) 0x10;
	end_of_mem_segment = (uint8_t *) 0x20;
	TEST_ASSERT_TRUE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			10)
		);


	start_of_mem_segment = (uint8_t *) 0x10;
	end_of_mem_segment = (uint8_t *) 0x20;
	TEST_ASSERT_FALSE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			30)
		);

	start_of_mem_segment = (uint8_t *) 0x20;
	end_of_mem_segment = (uint8_t *) 0x10;
	TEST_ASSERT_FALSE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			30)
		);

	start_of_mem_segment = (uint8_t *) 0x10;
	end_of_mem_segment = (uint8_t *) 0x10;
	TEST_ASSERT_FALSE(
		check_if_enought_mem_is_available(
			start_of_mem_segment,
			end_of_mem_segment,
			1)
		);
}

void test_write_meta_data(void)
{
	struct mem_meta_data m;
	memset(&m, 0, sizeof(m));

	write_meta_data(&m, (uint8_t *) 0x12, 8);

	TEST_ASSERT_EQUAL_PTR((uint8_t *) 0x12,
			      m.start_of_mem_segment);

	TEST_ASSERT_EQUAL_size_t(m.mem_segment_size, 8);
}

void test_basic_mem_allocator_malloc_static(void)
{
	uint8_t mem[128];
	struct mem_allocator *m = helper_get_mem_alloc_struct(mem, 
						       sizeof(mem));
	TEST_ASSERT_NOT_NULL(m);


	void *p1 = mem_allocator_malloc_static(m, 7);
	TEST_ASSERT_NOT_NULL(p1);
	TEST_ASSERT_EQUAL_size_t(sizeof(mem), m->memory_size);
	TEST_ASSERT_EQUAL_PTR(mem, m->memory);

	uint8_t *free_mem_ptr_after_first_mem_alloc = 
		mem + 7 + OVERFLOW_DETECTION_BUFF_SIZE;
	TEST_ASSERT_EQUAL_PTR(free_mem_ptr_after_first_mem_alloc,
			      m->start_of_free_mem);

	uint8_t *end_of_meta_data_ptr_after_first_mem_alloc = 
		mem + sizeof(mem) - sizeof(struct mem_meta_data);
	TEST_ASSERT_EQUAL_PTR(
		end_of_meta_data_ptr_after_first_mem_alloc,
		m->end_of_meta_data);

	TEST_ASSERT_EQUAL_size_t(7, 
			      m->end_of_meta_data->mem_segment_size);
	TEST_ASSERT_EQUAL_PTR(mem,
			      m->end_of_meta_data->start_of_mem_segment);

	void *p2 = mem_allocator_malloc_static(m, 8);
	TEST_ASSERT_NOT_NULL(p2);
	TEST_ASSERT_EQUAL_size_t(sizeof(mem), m->memory_size);
	TEST_ASSERT_EQUAL_PTR(mem, m->memory);

	uint8_t *free_mem_ptr_after_second_mem_alloc = 
		addr_allign_up(
			free_mem_ptr_after_first_mem_alloc,
			sizeof(void *))
		+ 8 + OVERFLOW_DETECTION_BUFF_SIZE;
	TEST_ASSERT_EQUAL_PTR(free_mem_ptr_after_second_mem_alloc,
			      m->start_of_free_mem);

	uint8_t *end_of_meta_data_ptr_after_second_mem_alloc = 
		mem + sizeof(mem) - 2*sizeof(struct mem_meta_data);
	TEST_ASSERT_EQUAL_PTR(
		end_of_meta_data_ptr_after_second_mem_alloc,
		m->end_of_meta_data);

	TEST_ASSERT_EQUAL_size_t(8, 
			      m->end_of_meta_data->mem_segment_size);
	TEST_ASSERT_EQUAL_PTR(addr_allign_up(
				      free_mem_ptr_after_first_mem_alloc,
				      sizeof(void *)),
				      m->end_of_meta_data->start_of_mem_segment);


	void *p3 = mem_allocator_malloc_static(m, 4);
	TEST_ASSERT_NOT_NULL(p3);
	TEST_ASSERT_EQUAL_size_t(sizeof(mem), m->memory_size);
	TEST_ASSERT_EQUAL_PTR(mem, m->memory);

	uint8_t *free_mem_ptr_after_third_mem_alloc = 
		free_mem_ptr_after_second_mem_alloc 
		+ 4 + OVERFLOW_DETECTION_BUFF_SIZE;
	TEST_ASSERT_EQUAL_PTR(free_mem_ptr_after_third_mem_alloc,
			      m->start_of_free_mem);

	uint8_t *end_of_meta_data_ptr_after_third_mem_alloc = 
		mem + sizeof(mem) - 3*sizeof(struct mem_meta_data);

	TEST_ASSERT_EQUAL_PTR(
		end_of_meta_data_ptr_after_third_mem_alloc,
		m->end_of_meta_data);

	free(m);
}

void test_full_mem_allocator_malloc_static(void)
{
	uint8_t mem[64];
	struct mem_allocator *m = helper_get_mem_alloc_struct(mem, 
						       sizeof(mem));
	TEST_ASSERT_NOT_NULL(m);

	void *p1 = mem_allocator_malloc_static(m, sizeof(mem));
	TEST_ASSERT_NULL(p1);

	size_t memory_used = sizeof(void *) 
		+ sizeof(struct mem_meta_data);

	void *p2 = mem_allocator_malloc_static(m, 64 - memory_used);
	TEST_ASSERT_NOT_NULL(p2);

	m->start_of_free_mem = mem;
	m->end_of_meta_data = (struct mem_meta_data *)
		((uint8_t *) mem + sizeof(mem));

	void *p3 = mem_allocator_malloc_static(m, 8);
	TEST_ASSERT_NOT_NULL(p3);

	size_t memory_used2 = sizeof(void *) 
		+ sizeof(struct mem_meta_data)
		+ 8
		+ sizeof(void *) 
		+ sizeof(struct mem_meta_data);

	void *p4 = mem_allocator_malloc_static(m, 64 - memory_used2);

	TEST_ASSERT_NOT_NULL(p4);

	// mem is full, so getting even 1byte should fail :)
	void *p5 = mem_allocator_malloc_static(m, 1);
	TEST_ASSERT_NULL(p5);

	free(m);
}

void test_mem_allocator_init_foo_args(void)
{
	uint8_t mem1[8];

	struct mem_allocator *m1 = 
		mem_allocator_init(NULL, 5);
	TEST_ASSERT_NULL(m1);

	// mem size too small
	struct mem_allocator *m2 = 
		mem_allocator_init(mem1, sizeof(mem1));
	TEST_ASSERT_NULL(m2);

}

void test_mem_allocator_init(void)
{
	uint8_t mem1[1024];
	memset(mem1, 0, sizeof(mem1));

	struct mem_allocator *m1 = 
		mem_allocator_init(mem1, sizeof(mem1));

	TEST_ASSERT_NOT_NULL(m1);
	TEST_ASSERT_EQUAL_PTR(mem1, m1);
	TEST_ASSERT_EQUAL_size_t(sizeof(mem1), m1->memory_size);

	uint8_t *expected_free_mem_ptr = mem1 
		+ sizeof(struct mem_allocator)
		+ OVERFLOW_DETECTION_BUFF_SIZE;
	TEST_ASSERT_EQUAL_PTR(expected_free_mem_ptr,
			      m1->start_of_free_mem);

	uint8_t *expected_end_of_meta_ptr = mem1 + sizeof(mem1) 
		- sizeof(struct mem_meta_data);
	TEST_ASSERT_EQUAL_PTR(expected_end_of_meta_ptr,
			      m1->end_of_meta_data);

	memset(mem1, 0, sizeof(mem1));
	uint8_t *unaligned_mem = ((uint8_t *) &mem1[0]) + 1;

	struct mem_allocator *m2 = 
		mem_allocator_init(unaligned_mem, sizeof(mem1));

	TEST_ASSERT_NOT_NULL(m2);

	const uint8_t * expected_mem_allocator_addr =
		(uint8_t *) mem1 + sizeof(void *);

	TEST_ASSERT_EQUAL_PTR(expected_mem_allocator_addr, m2);	
	TEST_ASSERT_EQUAL_PTR(expected_mem_allocator_addr
			      + sizeof(struct mem_allocator)
			      + OVERFLOW_DETECTION_BUFF_SIZE,
			      m2->start_of_free_mem);

	// when memory address is not alligned, 
	// memory size needs to be adjusted too, because alligning will
	// cause of a loss of few bytes.
	// mem_allocator_init function always decrease size by size of
	// arcitecture default type, if the memory address is not 
	// alligned, so the impelementation is simpler.
	TEST_ASSERT_EQUAL_size_t(sizeof(mem1)
				 - sizeof(void *), m2->memory_size);
}

void test_mem_allocator_set_lock(void)
{
	uint8_t mem[64];
	struct mem_allocator *m = helper_get_mem_alloc_struct(mem, 
						       sizeof(mem));

	mem_allocator_set_lock(m);

	TEST_ASSERT_EQUAL_UINT8(true, m->mem_lock);

	void *p = mem_allocator_malloc_static(m, 1);
	TEST_ASSERT_NULL(p);

	free(m);
}

void test_mem_allocator_set_overflow_detection_buff(void)
{

	uint8_t overflow_detection_buff[sizeof(void *) + 2];

	memset(overflow_detection_buff, 
	       0xff, sizeof(overflow_detection_buff));

	set_overflow_detection_buff(&overflow_detection_buff[1]);

	TEST_ASSERT_EQUAL_UINT8(0xff, overflow_detection_buff[0]);

	for (uint32_t i = 0; sizeof(void *) > i; i++)
	{
		TEST_ASSERT_EQUAL_UINT8((sizeof(void *)-1) - i, 
					overflow_detection_buff[i+1]);
	}

	uint8_t last_byte_index = sizeof(overflow_detection_buff)-1;
	TEST_ASSERT_EQUAL_UINT8(
		0xff, 
		overflow_detection_buff[last_byte_index]);
}

void test_mem_allocator_check_if_overflow_detection_buff_is_valid(void)
{

	uint8_t overflow_detection_buff[sizeof(void *) + 2];

	memset(overflow_detection_buff, 
	       0xff, sizeof(overflow_detection_buff));

	TEST_ASSERT_FALSE(
		check_if_overflow_detection_buff_is_valid(
			&overflow_detection_buff[1]));


	set_overflow_detection_buff(&overflow_detection_buff[1]);

	TEST_ASSERT_TRUE(
		check_if_overflow_detection_buff_is_valid(
			&overflow_detection_buff[1]));
}

void test_mem_allocator_check_for_overflow(void)
{
	uint8_t mem1[1024];

	struct mem_allocator *m = 
		mem_allocator_init(mem1, sizeof(mem1));

	TEST_ASSERT_NOT_NULL(m);

	void *p1 = mem_allocator_malloc_static(m, 12);
	TEST_ASSERT_NOT_NULL(p1);

	TEST_ASSERT_FALSE(
		mem_allocator_check_for_overflow(m));

	void *p2 = mem_allocator_malloc_static(m, 24);
	TEST_ASSERT_NOT_NULL(p2);

	TEST_ASSERT_FALSE(
		mem_allocator_check_for_overflow(m));


	void *p3 = mem_allocator_malloc_static(m, 4);
	TEST_ASSERT_NOT_NULL(p3);

	struct mem_meta_data *mt = 
		m->end_of_meta_data + 1;

	*(mt->start_of_mem_segment + mt->mem_segment_size+1) += 1;

	TEST_ASSERT_TRUE(
		mem_allocator_check_for_overflow(m));
}

void test_mem_allocator_usage_scenarion_01(void)
{
	uint8_t mem[1024];
	memset(mem, 0, sizeof(mem));

	struct mem_allocator *m = 
		mem_allocator_init(mem, sizeof(mem));

	TEST_ASSERT_NOT_NULL(m);

	const size_t p1_size = 8;
	uint8_t *p1 = mem_allocator_malloc_static(m, p1_size);
	TEST_ASSERT_NOT_NULL(p1);
	memset(p1, 0x55, p1_size);

	const size_t p2_size = 20;
	uint8_t *p2= mem_allocator_malloc_static(m, p2_size);
	TEST_ASSERT_NOT_NULL(p2);
	memset(p2, 0xAA, p2_size);

	const size_t p3_size = 512;
	uint8_t *p3= mem_allocator_malloc_static(m, p3_size);
	TEST_ASSERT_NOT_NULL(p3);
	memset(p3, 0xA0, p3_size);

	const size_t p4_size = 256;
	uint8_t *p4= mem_allocator_malloc_static(m, p4_size);
	TEST_ASSERT_NOT_NULL(p4);

	memset(p4, 0x50, p4_size);

	for (uint32_t i = 0; p1_size > i; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(0x55, p1[i]);
	}

	for (uint32_t i = 0; p2_size > i; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(0xAA, p2[i]);
	}

	for (uint32_t i = 0; p3_size > i; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(0xA0, p3[i]);
	}

	for (uint32_t i = 0; p4_size > i; i++)
	{
		TEST_ASSERT_EQUAL_UINT8(0x50, p4[i]);
	}


	TEST_ASSERT_FALSE(
		mem_allocator_check_for_overflow(m));

	p2[22] += 1;

	TEST_ASSERT_TRUE(
		mem_allocator_check_for_overflow(m));
}

//TODO: Test additional usage scenarios...
