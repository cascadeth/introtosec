////////////////////////////////////////////////////////////////////////
/*
______ _               _   _                
|  _  (_)             | | (_)               
| | | |_ _ __ ___  ___| |_ ___   _____  ___ 
| | | | | '__/ _ \/ __| __| \ \ / / _ \/ __|
| |/ /| | | |  __/ (__| |_| |\ V /  __/\__ \
|___/ |_|_|  \___|\___|\__|_| \_/ \___||___/ 
*/
////////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdint.h>

#include"bloom.h"

#define FILTER_SIZE 6000000             // 6 million bits
#define FNV_OFFSET_BASIS 2166136261     // 32 bit FNV offset
#define FNV_PRIME 16777619              // 32 bit FNV prime
#define POLY_PRIME 420757               // Hand selected prime for large polynomial


// Used to print filter if ever needed...
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 



////////////////////////////////////////////////////////////////////////
/*
______ ______   ______                _   _                 
| ___ \|  ___|  |  ___|              | | (_)                
| |_/ /| |_     | |_ _   _ _ __   ___| |_ _  ___  _ __  ___ 
| ___ \|  _|    |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
| |_/ /| |_     | | | |_| | | | | (__| |_| | (_) | | | \__ \
\____(_)_(_)    \_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
*/
////////////////////////////////////////////////////////////////////////

// A linked list of pointers to hash functions to use
struct bloom_hash_list {

    hash_function func;
    struct bloom_hash_list* next;
};

// The bloom filter structure itself
struct bloom_filter {

    struct bloom_hash_list* func;

    void *mem_bits;
    size_t size;
};

// Create a bloom filter structure with given size
bloom_t b_filter_init(size_t size) {

    bloom_t res = calloc(1, sizeof(struct bloom_filter));
    
	res->size = size;
    res->mem_bits = malloc(size);
    
	return res;
}

// Should free memory, but is busted currently...
void b_filter_mem_free(bloom_t filter) {

    printf("In filter free:\n\n");

    if (filter) {

        printf("DEBUG: Found filter.\n\n");

        struct bloom_hash_list* h;

        while (filter->func != NULL) {

            printf("DEBUG: Freeing function pointer...\n");

            filter->func = h->next;
            free(h);
        }

        printf("DEBUG: Freeing filter bits...\n");
        free(filter->mem_bits);
        printf("DEBUG: Freeing filter struct...\n");
        free(filter);
    }
}

// Add a function to the linked list of hash fuctions to use
void b_filter_add_hash_function(bloom_t filter, hash_function func) {

	struct bloom_hash_list* h = calloc(1, sizeof(struct bloom_hash_list));
	h->func = func;
    struct bloom_hash_list* last = filter->func;
    
	while (last && last->next) {

		last = last->next;
    }
    
	if (last) {
		last->next = h;
    } 
    else {
		filter->func = h;
	}
}

// Add an element to the filter
void b_filter_add_element(bloom_t filter, const void* element) {

	struct bloom_hash_list* h = filter->func;
    uint8_t* mem_bits = filter->mem_bits;
    
	while (h) {

		unsigned int hash = h->func(element);
		hash %= filter->size * 8;
		mem_bits[hash / 8] |= 1 << hash % 8;
		h = h->next;
	}
}

// Test if the element is in the filter
//      TRUE  if it could be in the filter
//      FALSE if definitely not in the filter
bool b_filter_test_element(bloom_t filter, const void* element) {

	struct bloom_hash_list* h = filter->func;
    uint8_t* mem_bits = filter->mem_bits;
    
	while (h) {

		unsigned int hash = h->func(element);
        hash %= filter->size * 8;
        
		if (!(mem_bits[hash / 8] & 1 << hash % 8)) {

			return false;
        }
        
		h = h->next;
    }
    
	return true;
}



////////////////////////////////////////////////////////////////////////
/*
 _   _           _         ______                _   _                 
| | | |         | |        |  ___|              | | (_)                
| |_| | __ _ ___| |__      | |_ _   _ _ __   ___| |_ _  ___  _ __  ___ 
|  _  |/ _` / __| '_ \     |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
| | | | (_| \__ \ | | |    | | | |_| | | | | (__| |_| | (_) | | | \__ \
\_| |_/\__,_|___/_| |_|    \_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
*/
////////////////////////////////////////////////////////////////////////

// Dan Bernstein hash
unsigned int djb2_h(const void* _str) {

	const char *str = _str;
	unsigned int hash = 5381;
    char c;
    
	while ((c = *str++)) {

		hash = ((hash << 5) + hash) + c;
	}
	return hash;
}

// Bob Jenkins one-at-a-time hash function
unsigned int jenkins_h(const void* _str) {
	const char *key = _str;
	unsigned int hash, i;
	while (*key) {
		hash += *key;
		hash += (hash << 10);
		hash ^= (hash >> 6);
		key++;
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

// Fowler-Noll-Vo (FNV-1a iteration) hash function
unsigned int FNV1a_h(const void* _str) {
    
    const char *str = _str;
    unsigned int hash = FNV_OFFSET_BASIS; 

    while (*str) {
        
        hash *= *str;
        hash ^= FNV_PRIME;
        str++;
    }

    return hash;
}

// A public domain ndbm hash
unsigned int sdbm_h(const void* _str) {

    const char *str = _str;
    unsigned int hash = 0;
    int c;

    while (c = *str++)
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash;
}

// Custom prime polynomial hash
unsigned int polynomial_h(const void* _str) {

    const char *str = _str;
    unsigned int hash = 0;
    size_t length = strlen(str);
    int i;

    for (i = (length - 1); i >= 0; i--) {

        hash = ( (str[i] + POLY_PRIME*hash) % FILTER_SIZE );
    }

    return hash;
}

// Remove a char from a string
void remove_char(char* str, char chr) {

    int i, j = 0;
    for ( i = 0; str[i] != '\0'; i++ ) {  // Loop through the original string
    
        if ( str[i] != chr )
            str[j++] = str[i];              // j only moves after we write a non-'chr' character to the current spot
    }

    str[j] = '\0';    // re-null-terminate
}


////////////////////////////////////////////////////////////////////////
/*
 _____                 ______                _   _                 
/  __ \                |  ___|              | | (_)                
| /  \/ ___  _ __ ___  | |_ _   _ _ __   ___| |_ _  ___  _ __  ___ 
| |    / _ \| '__/ _ \ |  _| | | | '_ \ / __| __| |/ _ \| '_ \/ __|
| \__/\ (_) | | |  __/ | | | |_| | | | | (__| |_| | (_) | | | \__ \
 \____/\___/|_|  \___| \_|  \__,_|_| |_|\___|\__|_|\___/|_| |_|___/
*/
////////////////////////////////////////////////////////////////////////

// Run bloom filter creation, adding->hashing dictionary, and testing input against filter
void run_hash_filter(char* dict, char* input, char* out, char num_hash) {

    // File pointers to open the file names indicated in the parameters
    FILE* d_fp;             
    FILE* i_fp;
    FILE* o3_fp;
    FILE* o5_fp;
    FILE* gpo_fp = o3_fp;       // Initially, general purpose output file pointer points to where output3 file pointer points

    size_t length = 0;
    ssize_t read;
    int test_this_many = 0;

    char* element = NULL;       // An individual element read from the dictionary file

    // Setup bloom filters 

    printf("INFO: Setting up bloom filter (%c-hash)\n", num_hash);

    bloom_t bloom = b_filter_init(FILTER_SIZE);
    
	b_filter_add_hash_function(bloom, djb2_h);
	b_filter_add_hash_function(bloom, jenkins_h);
    b_filter_add_hash_function(bloom, FNV1a_h);
    if (num_hash == '5') {
        b_filter_add_hash_function(bloom, sdbm_h);
        b_filter_add_hash_function(bloom, polynomial_h);
        gpo_fp = o5_fp;         // Set to output5 file pointer
    }

    // Start adding elements from the file

    
    d_fp = fopen(dict, "r");     // Open dictionary file for reading of elements
    if (d_fp == NULL) {

        perror("ERROR: Cannot open dictionary file for reading!\n");
        exit(1);
    }

    printf("INFO: Adding elements from dictionary for filter (%c-hash)\n", num_hash);

    while ((read = getline(&element, &length, d_fp)) != -1) {   // While there are still lines to read from the file

        // Remove unworthy characters from the input line
        remove_char(element, ' ');
        remove_char(element, '\t');
        remove_char(element, '\n');

        // Add element to filter
        b_filter_add_element(bloom, element);
        //getchar();    // Simple pause
    }

    fclose(d_fp);

    /*
    // print the entire filter...
    for (int i = 0; i < FILTER_SIZE; i++) {

        unsigned char* byte = bloom->mem_bits;
        printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte[i]));
    }
    */
    i_fp = fopen(input, "r");     // Open input file for reading of test elements
    if (i_fp == NULL) {

        perror("ERROR: Cannot open input file for reading!\n");
        exit(1);
    }

    gpo_fp = fopen(out, "w");     // Open output file for writing of test results
    if (gpo_fp == NULL) {

        perror("ERROR: Cannot open output file for writing!\n");
        exit(1);
    }

    printf("INFO: Testing elements from input for filter (%c-hash)\n", num_hash);

    getline(&element, &length, i_fp);

    test_this_many = atoi(element);

    //printf("DEBUG: Getting %d inputs...\n", test_this_many);

    for (test_this_many != 0; test_this_many--;) {

        getline(&element, &length, i_fp);
        
        // Remove unworthy characters from the input line
        remove_char(element, ' ');
        remove_char(element, '\t');
        remove_char(element, '\n');

        // test element to filter
        if (b_filter_test_element(bloom, element)) {

            fprintf(gpo_fp, "maybe\n");
        }
        else {

            fprintf(gpo_fp, "no\n");
        }
        //getchar();    // Simple pause
    }

    fclose(i_fp);
    fclose(gpo_fp);

    //b_filter_mem_free(bloom);

    printf("\nINFO: Done!\n\n");
}



////////////////////////////////////////////////////////////////////////
/*
___  ___  ___  _____ _   _ 
|  \/  | / _ \|_   _| \ | |
| .  . |/ /_\ \ | | |  \| |
| |\/| ||  _  | | | | . ` |
| |  | || | | |_| |_| |\  |
\_|  |_/\_| |_/\___/\_| \_/
*/
////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    
    if (argc < 8) { // Check the value of argc. If not enough parameters have been passed, inform user and exit.

        fprintf(stderr, "Usage: ./bloom_filter -d [file] -i [file] -o [file 3hash] [file 5hash] \n\n");
        exit(0);
    }

    int c;                  // Character option to switch on command line switches
    char* dictionary_file;  // File name for the dictionary
    char* inputs_file;      // File name for inputs to test with
    char* output3_file;     // File name for output when using 3 hashes
    char* output5_file;     // ^    ^    ^   ^      ^    ^     5 hashes


    // Using getopt library to parse command line options
    while ((c = getopt (argc, argv, "d:i:o:")) != -1) {
        
        switch (c) {    // Switch on option c, 
                        // optarg contains the next argv argument
            case 'd':
                dictionary_file = optarg;
                break;
            case 'i':
                inputs_file = optarg;
                break;
            case 'o':
                output3_file = optarg;
                output5_file = argv[optind];    // one further argument ahead to grab 2nd output file name
                break;
            default:
                abort();
        }
    }

    run_hash_filter(dictionary_file, inputs_file, output3_file, '3');
    run_hash_filter(dictionary_file, inputs_file, output5_file, '5');

    return 0;
}
