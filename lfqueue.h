/*
 * queue.h
 *
 *  Created on: Aug 27, 2016
 *      Author: yonghaiyang
 */

#ifndef LFQUEUE_H_
#define LFQUEUE_H_

typedef struct lfqueue lfqueue_t;

lfqueue_t* lfqueue_create(int size);
int lfqueue_size(lfqueue_t* queue);
int lfqueue_len(lfqueue_t* queue);
int lfqueue_avail(lfqueue_t* queue);
int lfqueue_enqueue(lfqueue_t* queue,void* element);
void* lfqueue_dequeue(lfqueue_t* queue);
void lfqueue_destroy(lfqueue_t* queue);
void lfqueue_reset(void* q);

typedef void (*lfqueue_handler_t)(void* data);
lfqueue_t* lfqueue_init(int size,lfqueue_handler_t handler);


#endif /* LFQUEUE_H_ */
