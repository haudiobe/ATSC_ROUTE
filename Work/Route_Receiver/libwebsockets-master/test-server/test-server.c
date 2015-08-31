#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>
#include <pthread.h>
#include <unistd.h>

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
    void *(*start_routine)(void*), void *arg);

void * serviceThread()
{
        while(1){
                printf("Hello thread!!\n");
				usleep(500000);
        }
}

static int callback_http(struct libwebsocket_context * this,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    return 0;
}

static int callback_dumb_increment(struct libwebsocket_context * this,
                                   struct libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
   
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            // create a buffer to hold our response
            // it has to have some pre and post padding. You don't need to care
            // what comes there, libwebsockets will do everything for you. For more info see
            // http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n597
            unsigned char *buf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + len +
                                                         LWS_SEND_BUFFER_POST_PADDING);
           
            int i;
           
            // pointer to `void *in` holds the incomming request
            // we're just going to put it in reverse order and put it in `buf` with
            // correct offset. `len` holds length of the request.
            for (i=0; i < len; i++) {
                buf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i ] = ((char *) in)[i];
            }
           
            // log what we recieved and what we're going to send as a response.
            // that disco syntax `%.*s` is used to print just a part of our buffer
            // http://stackoverflow.com/questions/5189071/print-part-of-char-array
            printf("received data: %s, replying: %.*s\n", (char *) in, (int) len,
                 buf + LWS_SEND_BUFFER_PRE_PADDING);
           
            // send response
            // just notice that we have to tell where exactly our response starts. That's
            // why there's `buf[LWS_SEND_BUFFER_PRE_PADDING]` and how long it is.
            // we know that our response has the same length as request because
            // it's the same message in reverse order.
            libwebsocket_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
           
            // release memory back into the wild
            free(buf);
            break;
        }
        default:
            break;
    }
   
   
    return 0;
}

static struct libwebsocket_protocols protocols[] = {
    /* first protocol must always be HTTP handler */
    {
        "http-only",   // name
        callback_http, // callback
        0              // per_session_data_size
    },
    {
        "dumb-increment-protocol", // protocol name - very important!
        callback_dumb_increment,   // callback
        0                          // we don't use any per session data

    },
    {
        NULL, NULL, 0   /* End of list */
    }
};

int main(void) {
    // server url will be http://localhost:9000
    int port = 9000;
    const char *interface = NULL;
    struct libwebsocket_context *context;
    // we're not using ssl
    //const char *cert_path = NULL;
    //const char *key_path = NULL;
    // no special options
    int opts = 0;
   
   struct lws_context_creation_info info;

    pthread_t tid1;

    pthread_create(&tid1,NULL,serviceThread,NULL);
   
   memset(&info, 0, sizeof info);
   info.port = port;
   info.iface = interface;
   info.protocols = protocols;
   info.extensions = libwebsocket_get_internal_extensions();
   //if (!use_ssl) {
   info.ssl_cert_filepath = NULL;
   info.ssl_private_key_filepath = NULL;
   //} else {
   // info.ssl_cert_filepath = LOCAL_RESOURCE_PATH"/libwebsockets-test-server.pem";
   // info.ssl_private_key_filepath = LOCAL_RESOURCE_PATH"/libwebsockets-test-server.key.pem";
   //}
   info.gid = -1;
   info.uid = -1;
   info.options = opts;
   
   context = libwebsocket_create_context(&info);
   
    if (context == NULL) {
        fprintf(stderr, "libwebsocket init failed\n");
        return -1;
    }
   
    printf("starting server...\n");
   
    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while (1) {
        libwebsocket_service(context, 50);
        // libwebsocket_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }
   
    libwebsocket_context_destroy(context);
   
    return 0;
}




