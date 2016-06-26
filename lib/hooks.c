#include <stdio.h>
#include <stdlib.h>
#include "transfer.h"

extern void *__libc_malloc(size_t size);

int malloc_hook_active = 1;
int counter = 0;
int fd_shared; //pci device file descriptor
uint offset;
void *memptr; //base pointer of the shared memory
sem_t *sem;
int fd_fifo;

void* malloc (size_t size) {
	counter++;
	
	if (counter>4){
		input in;
		in.func = MALLOC;
		in.size = size;
    	sem_post(sem);
    	write(fd_fifo, &in, sizeof(input));
    	read(fd_fifo, &in, sizeof(input));
		return memptr+(1024*1024)+in.offset;
	}
	return __libc_malloc(size);	//glibc specific weak point
}

void *calloc(size_t nelem, size_t elsize){
	
}

void *realloc(void* ptr, size_t size){

}

void free (void* ptr){
	counter--;
	if (counter>4){
		input in;
		in.func = FREE;
		in.size = ptr-memptr-(1024*1024);
		sem_post(sem);
		write(fd_fifo, &in, sizeof(input));
    	read(fd_fifo, &in, sizeof(input));
	}
}