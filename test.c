/*
 * test.c
 *
 *  Created on: Aug 27, 2016
 *      Author: yonghaiyang
 */
#include "lfqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static volatile int s = 0;
static volatile int r = 0;

void handler(void* data){
	int *d = data;
	free(d);
	r+=1;
}

int main(){
	int* data;
	lfqueue_t* queue = lfqueue_init(1024,handler);
//	lfqueue_t* queue = lfqueue_create(1024);

	for(int i=0;i<40960;i++){
		data = malloc(sizeof(int));
		*data = i;
		if(lfqueue_enqueue(queue,data)){
//			printf("error:%d\n",i);
			free(data);
		}else{
			s += 1;
		}

		if(i%100==0)usleep(1);

//		printf("i:%d len:%d avail:%d\n",i,lfqueue_len(queue),lfqueue_avail(queue));
	}
	sleep(1);
	printf("s:%d r:%d\n",s,r);
	lfqueue_destroy(queue);
	return 0;
}
