
/*
 *  Author: Kristoffer Robin Stokke
 *          University of Oslo, Institute of Informatics
 *
 *  This file provides the interface to the host operating system. 
 *  */

#ifndef __TESTLIB__
#define __TESTLIB__

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/* Initializes the fake cuda library */
//int init();

#define FACUDA_SUCCESS 0
#define FACUDA_ERROR -1

/*
#include <stdint.h>         // Must include this header for uint8_t typedef
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
*/

/* Hello world. */
void testFunction(void);

/* Sends a message from msg_buf of size msg_sz to the IOListener.
 *
 * Returns FACUDA_SUCCESS on success or FACUDA_ERROR on failure. */
int sendMessage(void *msg_buf, size_t msg_sz);

/* Returns the number of bytes in the message and in *msg_buf a 
 * pointer to a received message. 
 *
 * Returns FACUDA_ERROR on failure. */
size_t recvMessage(void **msg_buf);

#endif
