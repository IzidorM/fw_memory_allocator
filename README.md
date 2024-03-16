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
static uint8_t mem[1024];
static struct mem_allocator *m = NULL;

void my_memory_allocator_init_simple(void)
{
	struct mem_allocator *m = mem_allocator_init(mem, sizeof(mem));
    assert(ma_normal);
}

void *my_malloc(size_t size)
{
	return mem_allocator_malloc_static(m, size);
}


```

### Usage when there are different sections of ram
``` c
#include <stdint.h>

//link to right part of ram
static uint8_t ram_normal[32 * 1024]; 
static struct mem_allocator *m_normal = NULL;

//link to right part of ram
static uint8_t ram_fast[1024];
static struct mem_allocator *m_fast = NULL;

//link to right part of ram
static uint8_t ram_deep_sleep_persistant[8 * 1024];
static struct mem_allocator *m_ds_persistant = NULL;

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

## Motivation for writing this library

We will skip the explanation of why static memory allocation is
typically used in embedded systems. Instead, we will explore the
shortcomings of static memory allocation and how to implement a
restricted form of dynamic memory allocation. This approach aims to
address the weaknesses of static memory allocation without introducing
the drawbacks of conventional dynamic memory allocation.

So there are 3 problems with static memory allocation that I want to
solve with this library:

1. Limits the use of opaque data types: In static allocation, the memory size of data structure needs to be known at compile time. Opaque data types (whose sizes are not known to the compiler) cannot be directly used with static memory allocation. 

2. Lacks buffer overflow detection: Buffer overflow errors can't be detected easily as static memory doesn't inherently provide such mechanisms.

3. Inefficient memory sectioning: With microcontrollers (MCUs) that have RAM divided into different sections (fast RAM, normal RAM, etc.), static memory allocation requires detailed configuration and tweaking of linker scripts to determine allocation of variables to different memory sections. 


## Contribute
Everybody is welcome to contribute.

* If you find a bug, open an issue or fix it and create a pull request
* if you have a feature request also open an issue or create a pull request

If you made changes to the code, add unit tests. The 100% unit test 
coverage is required.

### Unit tests
There are unit tests provided in the unit_test folder. 
Correcting the path to Unity in Makefile in needed before first
run. Other than that they should run out of the box on any linux
system with gcc installed. 



