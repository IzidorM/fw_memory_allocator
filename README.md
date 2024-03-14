# memory allocator
Simple implementation of a memory allocator. It provides overflow detection by enclosing each block with canary values. It doesnt provide memory free functionality. Its purpose is to substitute normal static memory allocation in embedded systems and provide some additional benefits which are hard to get when using static memory allocation. It provides locking mechanism that can be used to prevent using memory allocation after system initialization is done.

## Features
* Written in C
* Small and efficint code base
* Provides locking of memory allocation
* Support for multiple instances in case system offers different ram sections
* BSD license

## Usage

Memory allocator needs to be initialized before use, so we can have multiple instances if needed. 

### Simple usage when only one allocator is needed
``` c
#include <stdint.h>

// memory that allocator will work on
uint8_t mem[1024];
struct mem_allocator *m = NULL;

void my_memory_allocator_init_simple(void)
{
	struct mem_allocator *m = mem_allocator_init(mem, sizeof(mem));
    assert(ma_normal);
}

```

### Usage when there are different sections of ram
``` c
#include <stdint.h>

uint8_t ram_normal[32 * 1024];
struct mem_allocator *m_normal = NULL;

uint8_t ram_fast[1024];
struct mem_allocator *m_fast = NULL;

uint8_t ram_deep_sleep_persistant[8 * 1024];
struct mem_allocator *m_ds_persistant = NULL;

void my_memory_allocator_init_multiple_ram_sections(void)
{
	m_normal = 
		mem_allocator_init(ram_normal, sizeof(ram_normal));
	assert(m_normal);

	m_fast = mem_allocator_init(ram_fast, sizeof(ram_fast));
	assert(m_fast);

	m_ds_persistant = 
		mem_allocator_init(ram_deep_sleep_persistant, 
				   sizeof(ram_deep_sleep_persistant));

}

void *my_malloc_normal(size_t size)
{
	return mem_allocator_malloc_static(m_normal, size);
}

void *my_malloc_fast(size_t size)
{
	return mem_allocator_malloc_static(m_fast, size);
}

void *my_malloc_deep_sleep_safe(size_t size)
{
	return mem_allocator_malloc_static(m_ds_persistant, size);
}

```

### runtime checks for memory overflow
mem_allocator_malloc_static function adds a canary value at the end of the requested memory.
To check, if the memory overflow occured, user needs to call 

``` c
bool mem_allocator_check_for_overflow(struct mem_allocator *m)
```

The function returns true if the overflow was detected. This function should be called in periodic intervals. The period depends of the system and should be decided by the system developer...


## Unit tests
There are unit tests provided in the unit_test folder. 
Correcting the path to Unity in Makefile in needed before first
run. Other than that they should run out of the box on any linux
system with gcc installed. 



