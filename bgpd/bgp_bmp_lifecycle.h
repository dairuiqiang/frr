#ifndef BGP_BMP_LIFECYCLE_H
#define BGP_BMP_LIFECYCLE_H

#include <stdatomic.h>
#include <pthread.h>

// Structure for reference counting
typedef struct {
    atomic_int ref_count;
} RefCount;

// Structure for data copy
typedef struct {
    void *data;
    size_t size;
    RefCount ref_count;
} DataCopy;

// Function to initialize reference counting
static inline void init_ref_count(RefCount *rc) {
    atomic_init(&rc->ref_count, 1);
}

// Function to increase reference count
static inline void retain_ref(RefCount *rc) {
    atomic_fetch_add(&rc->ref_count, 1);
}

// Function to decrease reference count and free if zero
static inline void release_ref(RefCount *rc) {
    if (atomic_fetch_sub(&rc->ref_count, 1) == 1) {
        // Free resources here if needed
    }
}

// Function to create a data copy
DataCopy *create_data_copy(void *src_data, size_t size) {
    DataCopy *copy = malloc(sizeof(DataCopy));
    if (!copy) return NULL;
    copy->data = malloc(size);
    if (!copy->data) {
        free(copy);
        return NULL;
    }
    memcpy(copy->data, src_data, size);
    copy->size = size;
    init_ref_count(&copy->ref_count);
    return copy;
}

// Function to free data copy
void free_data_copy(DataCopy *copy) {
    if (copy != NULL) {
        release_ref(&copy->ref_count);
        if (atomic_load(&copy->ref_count.ref_count) == 0) {
            free(copy->data);
            free(copy);
        }
    }
}

#endif // BGP_BMP_LIFECYCLE_H
