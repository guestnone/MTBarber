#pragma once

#include<pthread.h>

typedef struct cond_queue
{
    pthread_cond_t cond;
    int nr;
    struct cond_queue* next;
} cond_queue;

struct cond_queue* getLastElementOfCondQueue(struct cond_queue **c);
struct cond_queue* getFirstElementOfCondQueue(struct cond_queue **c);
int getcondQueueSize(struct cond_queue* c);


struct cond_queue* addElementtoCondQueue(struct cond_queue **c,int nr);
struct cond_queue* popCondQueueFront(struct cond_queue *c);
int isCondQueueEmpty(struct cond_queue* c);
void printQueue(struct cond_queue *c);
