
#ifndef TRANSFER_VIRTIO_SERIAL
#define TRANSFER_VIRTIO_SERIAL

/* This transfer library uses the virtio-serial device
 * integrated in qemu (hw/datatransfer-serial.c) to 
 * transfer data between host and guest. */

#include "transfer.h"

#define CHAR_DEV "/dev/virtio-ports/robineier"


static FILE *device_node;
int fd;     // Device file descriptor

void testFunction(){
    printf("testFunction: Hello world!\n");
}


//void init(void) __attribute__((constructor));
void __attribute__((constructor)) init(void){

    device_node = fopen(CHAR_DEV, "r+");
    if(device_node == NULL){
        fprintf(stderr, "Failed opening the device node.\n");
        exit(EXIT_FAILURE);
    }

    fd = fileno(device_node);

}

//void deinit(void) __attribute__((destructor));
void __attribute((destructor)) deinit(void){

    close(fd);

}


/* Sends a message to the device. 
 *
 * Returns the number of bytes sent or a negative value in
 * case of an error. */
int sendMessage(void *msg_buf, size_t len){

    //fprintf(stdout, "Writing message with fd %d\n", fd);

    uint32_t msg_sz = (uint32_t) len;
    uint32_t toWrite = msg_sz;
    uint32_t written;

    uint8_t* readPos = (uint8_t*)msg_buf;

    if(write(fd, &msg_sz, 4) < 4)
        return FACUDA_ERROR;

    while(toWrite != 0){

        written = write(fd, readPos, toWrite);
        if(written <= 0)
            return FACUDA_ERROR;
        toWrite -= written;
        readPos += written;
    }

    return msg_sz;
}

/* Receives a message from the virtual device. 
 *
 * Returns the size of the message and in *msg_buf a pointer
 * to the message. (In case of an error a negative value is
 * returned an no data is put in the buffer. )
 *
 * The user is responsible for freeing the buffer after use. */
size_t recvMessage(void **msg_buf){
    
    // First read the message size
    uint32_t msg_sz;
    uint32_t toRead;
    uint32_t justRead = 0;

    uint8_t* writePos;

    //fprintf(stdout, "Reading size with fd %d\n", fd);

    if(read(fd, &msg_sz, 4) < 4){
        perror("");
        return FACUDA_ERROR;
    }
    
    //fprintf(stdout, "Got response message of size %d\n", msg_sz);
    if(msg_sz == 0){
        *msg_buf = NULL;
        return 0;
    }
    toRead = msg_sz;
    
    // msg_sz should now contain the message size in bytes

    *msg_buf = malloc(msg_sz);
    writePos = (uint8_t*)*msg_buf;
    while(toRead != 0){
        
        justRead = read(fd, writePos, (int)toRead);
        if(read <= 0)
            return FACUDA_ERROR;
        toRead -= justRead;
        writePos += justRead;
    }

    return (size_t)msg_sz;
}

#endif
