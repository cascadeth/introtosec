#ifndef _BLOOM_H
#define _BLOOM_H
#include <stddef.h>
#include <stdbool.h>

typedef unsigned int (*hash_function)(const void *data);
typedef struct bloom_filter * bloom_t;

// Creates a new bloom filter with no hash functions and user defined size
bloom_t b_filter_init(size_t size);

// Frees a bloom filter(or rather is supposed to...)
void b_filter_mem_free(bloom_t filter);

// Adds a hashing function to the bloom filter
void b_filter_add_hash_function(bloom_t filter, hash_function func);

// Adds an element to the bloom filter
void b_filter_add_element(bloom_t filter, const void *element);

// Tests if an element is in the bloom filter
//     TRUE  if it could have been added
//     FALSE if it was for sure not added
bool b_filter_test_element(bloom_t filter, const void *element);

// HASH FUNCTIONS

unsigned int djb2_h(const void *_str);
unsigned int jenkins_h(const void *_str);
unsigned int FNV1a_h(const void *_str);
unsigned int sdbm_h(const void *_str);
unsigned int polynomial_h(const void *_str);

#endif