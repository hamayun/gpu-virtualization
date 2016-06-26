
#ifndef TRANSFER
#define TRANSFER

#include "transfer.h"

#define CHAR_DEV "/dev/cudamodule0"


static FILE *device_node;
int fd;     // Device file descriptor

void testFunction(){
    printf("testFunction: Hello world!\n");
}

int init(){

    device_node = fopen(CHAR_DEV, "r+");
    if(device_node == NULL){
        fprintf(stderr, "Failed opening the device node.\n");
        return FACUDA_ERROR;
    }

    fd = fileno(device_node);

    return FACUDA_SUCCESS;
}

/* Sends a message to the device. 
 *
 * Returns the number of bytes sent or a negative value in
 * case of an error. */
int sendMessage(void *msg_buf, size_t msg_sz){

    //fprintf(stdout, "Writing message with fd %d\n", fd);

    if(write(fd, msg_buf, msg_sz) < msg_sz)
        return FACUDA_ERROR;

    return msg_sz;
}

#endif
