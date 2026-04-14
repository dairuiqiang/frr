/**
 * bmp_bmp_queue.c
 * BMP Event Queue Implementation
 * 
 * This file implements a thread-safe queue for BMP events using mutex and atomic operations.
 */

#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct bmp_event {
    struct bmp_event *next;
    // Add additional event data fields here
} bmp_event_t;

typedef struct bmp_event_queue {
    bmp_event_t *head;
    bmp_event_t *tail;
    pthread_mutex_t mutex;
    atomic_int count;
} bmp_event_queue_t;

void bmp_queue_init(bmp_event_queue_t *queue) {
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    atomic_store(&queue->count, 0);
}

void bmp_enqueue(bmp_event_queue_t *queue, bmp_event_t *event) {
    pthread_mutex_lock(&queue->mutex);
    event->next = NULL;
    if (queue->tail) {
        queue->tail->next = event;
    } else {
        queue->head = event;
    }
    queue->tail = event;
    atomic_fetch_add(&queue->count, 1);
    pthread_mutex_unlock(&queue->mutex);
}

bmp_event_t *bmp_dequeue(bmp_event_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    bmp_event_t *event = queue->head;
    if (event) {
        queue->head = event->next;
        if (!queue->head) {
            queue->tail = NULL;
        }
        atomic_fetch_sub(&queue->count, 1);
    }
    pthread_mutex_unlock(&queue->mutex);
    return event;
}

int bmp_queue_count(bmp_event_queue_t *queue) {
    return atomic_load(&queue->count);
}

void bmp_queue_destroy(bmp_event_queue_t *queue) {
    pthread_mutex_destroy(&queue->mutex);
}
