
#include "datatransfer-serial.h"
#include <time.h>
#include "include/qemu/osdep.h"
#include "include/qemu/typedefs.h"
#include "include/glib-compat.h"
#include "include/sysemu/char.h"
#include "include/qemu/error-report.h"
#include "include/trace.h"
#include "include/hw/virtio/virtio-serial.h"

typedef struct serialDevice{
    VirtIOSerialPort port;
} DataTransfer;

typedef struct transfState_t{
    uint32_t transfSize;
} transfState_t;

transfState_t *transfState;

static int dtransf_init(VirtIOSerialPort*);
static void dtransf_class_init(ObjectClass*, void*);
static int dtransf_exit(VirtIOSerialPort*);
static void dtransf_open(VirtIOSerialPort*,int);
static void dtransf_grdy(VirtIOSerialPort*);
static ssize_t dtransf_gwrite(VirtIOSerialPort *port, const uint8_t *buf, size_t len);
static void dtransf_realized(DeviceState *de, Error **err);
static void dtransf_unrealized(DeviceState *de, Error **err);

struct replythread_args{
    VirtIOSerialPort *port;
    size_t len;
};

pthread_attr_t rt_attrs;
pthread_t replythread;
ReceivedDataHandler *datahandler;
//clock_t begin, end;
//double time_spent;

dtransError_t registerDataHandler(ReceivedDataHandler *handler){
    datahandler = handler;
    return DTRANS_SUCCESS;
}

static void* thread_reply(void* args){

    struct replythread_args *replyargs = args;
    uint32_t respMsgSz = 0;
    ssize_t size;
    datahandler((uint32_t)replyargs->len, &respMsgSz);
    while(virtio_serial_guest_ready(replyargs->port) == 0);

    size = virtio_serial_write(replyargs->port, (const uint8_t*)&respMsgSz, 4);
    //end = clock();
    //time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    //printf("\ntime : %f", time_spent);
    free(args);
    
    pthread_exit(NULL);
}

static void dtransf_realized(DeviceState *de, Error **err) {

}

static void dtransf_unrealized(DeviceState *de, Error **err) {

}

static int dtransf_init(VirtIOSerialPort* port){
    return 0;
}

static int dtransf_exit(VirtIOSerialPort* port){
    return 0;
}

static void dtransf_open(VirtIOSerialPort* port, int connected){

    if (connected){
        if(virtio_serial_open(port) != 0)
            fprintf(stderr, "virtio_serial_open failed!\n");
    }
    else {
       if(virtio_serial_close(port) != 0)
            fprintf(stderr, "virtio_serial_close failed!\n");
    }

}

static void dtransf_grdy(VirtIOSerialPort* port){

}

static ssize_t dtransf_gwrite(VirtIOSerialPort *port, const uint8_t *buf, size_t len){
    
    uint32_t transferSize = *((uint32_t*) buf);
    transfState->transfSize = transferSize;
    struct replythread_args *replyargs = malloc(sizeof(struct replythread_args));
    replyargs->port = port;
    replyargs->len = transfState->transfSize;
    transfState->transfSize = 0;

    if(pthread_create(&replythread, &rt_attrs, thread_reply, (void*) replyargs) != 0){
        fprintf(stderr, "Unable to create response handler thread!\n");
    }

    return len;
}

static Property datatransfer_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void dtransf_class_init(ObjectClass *klass, void* data){
  
    pthread_attr_init(&rt_attrs);
    pthread_attr_setdetachstate(&rt_attrs, PTHREAD_CREATE_DETACHED);

    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtIOSerialPortClass *k = VIRTIO_SERIAL_PORT_CLASS(klass);

    k->realize          =   dtransf_realized;
    k->unrealize        =   dtransf_unrealized;
    k->set_guest_connected      =   dtransf_open;
    k->guest_ready      =   dtransf_grdy;
    k->have_data        =   dtransf_gwrite;
    k->guest_writable   =   dtransf_init;
    dc->props           =   datatransfer_properties;

    transfState         =   malloc(sizeof(transfState_t));

    transfState->transfSize     = 0;
}

static TypeInfo serialDeviceInfo = {

    .name           =   "datatransfer-serial",
    .parent         =   TYPE_VIRTIO_SERIAL_PORT,
    .instance_size  =   sizeof(DataTransfer),
    .class_init     =   dtransf_class_init,
};

static void dtransf_register_types(void){

    type_register_static(&serialDeviceInfo);
}
type_init(dtransf_register_types)



