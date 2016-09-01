#include "lfqueue.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

typedef struct lfqueue{
	int rpos;
	int wpos;
	int size;

	pthread_t tid;
	bool running;
	sem_t* sem;
	lfqueue_handler_t handler;

	void* buffer[0];
}lfqueue_t;

#define SIZE(q) ((q)->size)
#define RPOS(q) ((q)->rpos)
#define WPOS(q) ((q)->wpos)
#define LEN(q)  (WPOS(q) >= RPOS(q) ? WPOS(q) - RPOS(q) : SIZE(q) + WPOS(q) - RPOS(q))
#define IS_EMPTY(q) (WPOS(q) == RPOS(q))
#define AVAIL(q) (SIZE(q) - LEN(q) - 1)
#define IS_FULL(q) (((WPOS(q) + 1) & (SIZE(q) - 1)) == RPOS(q))
#define STEPR(q) (RPOS(q) = (RPOS(q) + 1) & (SIZE(q) - 1))
#define STEPW(q) (WPOS(q) = (WPOS(q) + 1) & (SIZE(q) - 1))
#define RESET(q) (WPOS(q) = RPOS(q) = 0)
#define ENQUEUE(q,e) ((q)->buffer[WPOS(q)] = e,STEPW(q))
#define DEQUEUE(q,e) (e = (q)->buffer[RPOS(q)],(q)->buffer[RPOS(q)] = NULL,STEPR(q))

#define WAIT(q) sem_wait((q)->sem)
#define SIGNAL(q) sem_post((q)->sem)

static void*  reader_thread(void* data){
	lfqueue_t* queue = data;
	int* d;
	while(queue->running){
		WAIT(queue);
		if(!queue->running)break;
		d  = lfqueue_dequeue(queue);
		while(d){
			if(queue->handler)queue->handler(d);
			d  = lfqueue_dequeue(queue);
		}
	}

	return NULL;
}

lfqueue_t* lfqueue_init(int size,lfqueue_handler_t handler){
	lfqueue_t* queue = lfqueue_create(size);
	if(!queue)return NULL;
	queue->handler = handler;

	queue->sem = malloc(sizeof(sem_t));
	sem_init(queue->sem,0,0);

	queue->running = true;
	pthread_create(&queue->tid,NULL,reader_thread,queue);
	return queue;
}

/**
 *size:must be pow(2,n)
 **/
lfqueue_t* lfqueue_create(int size){
	lfqueue_t*  queue = malloc(sizeof(lfqueue_t) + sizeof(void*)  * size);
	if(!queue)return NULL;

	memset(queue,0,sizeof(lfqueue_t) + sizeof(void*)  * size);
	queue->size = size;
	return queue;
}

int lfqueue_size(lfqueue_t* queue){
	if(!queue)return 0;	
	return SIZE(queue);
}

int lfqueue_len(lfqueue_t* queue){
	if(!queue)return 0;
	return LEN(queue);
}

int lfqueue_avail(lfqueue_t* queue){
	if(!queue)return 0;
	return AVAIL(queue);
}


void lfqueue_reset(void* q){
	lfqueue_t * queue = q;
	if(!queue)return;
	RESET(queue);
}

int lfqueue_enqueue(lfqueue_t* queue,void* element){
	if(!queue)return -1;
	if(IS_FULL(queue))return -2;
	ENQUEUE(queue,element);
	if(queue->sem)SIGNAL(queue);
	return 0;
}

void* lfqueue_dequeue(lfqueue_t* queue){
	void* element;
	if(!queue)return NULL;
	if(IS_EMPTY(queue))return NULL;
	DEQUEUE(queue,element);
	return element;
}

void lfqueue_destroy(lfqueue_t* queue){
	void* ret;
	if(queue->tid){
		queue->running = false;
		SIGNAL(queue);
		pthread_join(queue->tid,&ret);
		free(queue->sem);
	}

	if(queue)free(queue);
}
