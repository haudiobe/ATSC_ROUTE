
#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>
#include <pthread.h>
#include <unistd.h>
#include "../alclib/alc_rx.h"

extern struct packetBuffer *circularPacketBuffer;
extern unsigned long readPtr;
extern unsigned long writePtr;
extern unsigned int tunedIn;
extern unsigned int workingPort;

static int callback_http(struct libwebsocket_context * this,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    return 0;
}

// create a buffer to hold our response
// it has to have some pre and post padding. You don't need to care
// what comes there, libwebsockets will do everything for you. For more info see
// http://git.warmcat.com/cgi-bin/cgit/libwebsockets/tree/lib/libwebsockets.h#n597
unsigned char *respBuf = NULL;
size_t dataLenVideo = 32000;
uint32_t mcounter = 1;

unsigned char *respBufAudio = NULL;
size_t dataLenAudio = 3200;
uint32_t mcounterAudio = 1;

struct libwebsocket *wsiSave[2];//1 :video, 2 audio
static int fileCounter = 1;
static int callback_dumb_increment(struct libwebsocket_context * this,
                                   struct libwebsocket *wsi,
                                   enum libwebsocket_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
   
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("***************connection established\n");

            if(workingPort == 4001 || workingPort == 4003)
            {
                //FILE * tempff = fopen("mergedFile.mp4","w");
                //fclose(tempff);
            }
			// get the ball rolling
			usleep(10000);
			libwebsocket_callback_on_writable(this, wsi);
            break;
			
        case LWS_CALLBACK_RECEIVE: {
           
            int i;
			
			//respBuf[LWS_SEND_BUFFER_PRE_PADDING + dataLenVideo - 1] = '\0';
			//savelen = len;
           
            // pointer to `void *in` holds the incomming request
            // we're just going to put it in reverse order and put it in `respBuf` with
            // correct offset. `len` holds length of the request.
            //for (i=0; i < len; i++) {
            //    respBuf[LWS_SEND_BUFFER_PRE_PADDING + (len - 1) - i ] = ((char *) in)[i];
            //}
           
            // log what we recieved and what we're going to send as a response.
            // that disco syntax `%.*s` is used to print just a part of our buffer
            // http://stackoverflow.com/questions/5189071/print-part-of-char-array
            printf("received data: %s: %x, %x\n", (char *) in, this, wsi);

			if(strncmp((char *) in, "video",10) == 0)
			{
				respBuf = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + dataLenVideo +
															 LWS_SEND_BUFFER_POST_PADDING);
				
				memset(&respBuf[LWS_SEND_BUFFER_PRE_PADDING],44,dataLenVideo);
				wsiSave[0] = wsi;
			}
			
			if(strncmp((char *) in, "audio",10) == 0)
			{
				respBufAudio = (unsigned char*) malloc(LWS_SEND_BUFFER_PRE_PADDING + dataLenAudio+
															 LWS_SEND_BUFFER_POST_PADDING);
				
				memset(&respBufAudio[LWS_SEND_BUFFER_PRE_PADDING],44,dataLenAudio);
				wsiSave[1] = wsi;
			}
           
            // send response
            // just notice that we have to tell where exactly our response starts. That's
            // why there's `respBuf[LWS_SEND_BUFFER_PRE_PADDING]` and how long it is.
            // we know that our response has the same length as request because
            // it's the same message in reverse order.
            //libwebsocket_write(wsi, &respBuf[LWS_SEND_BUFFER_PRE_PADDING], len, LWS_WRITE_TEXT);
           
            // release memory back into the wild
            //free(buf);
            break;
        }
		
		case LWS_CALLBACK_SERVER_WRITEABLE:
			if(wsi == wsiSave[0])
			{
				if (respBuf == NULL) {
					// schedule ourselves to run next tick anyway
					libwebsocket_callback_on_writable(this, wsi);
					return 0;
				}
				uint32_t nwC = htonl (mcounter);
                struct packetBuffer toSend = newReadFromBuffer();
                if(toSend.length != 0)
                {
                    char packetFileName[1000];
				    memcpy(&respBuf[LWS_SEND_BUFFER_PRE_PADDING],toSend.buffer,toSend.length);
				    // send our asynchronous message LWS_WRITE_BINARY or LWS_WRITE_TEXT
				    libwebsocket_write(wsi, &respBuf[LWS_SEND_BUFFER_PRE_PADDING], toSend.length, LWS_WRITE_BINARY);

                    free(toSend.buffer);
                    //FILE * tempff = fopen("mergedFile.mp4","a");
                    
                    //fwrite(&respBuf[LWS_SEND_BUFFER_PRE_PADDING],1,toSend.length, tempff);
                    //fclose(tempff);

                    
                    sprintf(packetFileName,"packet%3d.mp4",fileCounter);
                    fileCounter++;
                    /*tempff = fopen(packetFileName,"wb");
                    fprintf(stderr,"************************ ptr2: %x\n",tempff);
                    fprintf(stdout,"************************ ptr2: %x\n",tempff);
                    fwrite(&respBuf[LWS_SEND_BUFFER_PRE_PADDING],1,toSend.length, tempff);
                    fclose(tempff);*/
                    //if(fileCounter > 2000)
                      //  exit(-1);

                    //fprintf(stderr,"************************ Done to write in %s, ptr: %x\n",packetFileName,tempff);
                    //fprintf(stdout,"************************ Done to write in %s, ptr: %x\n",packetFileName,tempff);
                }
			    // and schedule ourselves again
			    usleep(1000);
			    mcounter ++;
			}
			
			if(wsi == wsiSave[1])
			{
				if (respBufAudio == NULL) {
					// schedule ourselves to run next tick anyway
					libwebsocket_callback_on_writable(this, wsi);
					return 0;
				}
				uint32_t nwC = htonl (mcounterAudio);
                struct packetBuffer toSend = newReadFromBuffer();
                if(toSend.length != 0)
                {
				    memcpy(&respBufAudio[LWS_SEND_BUFFER_PRE_PADDING],toSend.buffer,toSend.length);
				    // send our asynchronous message LWS_WRITE_BINARY or LWS_WRITE_TEXT
				    libwebsocket_write(wsi, &respBufAudio[LWS_SEND_BUFFER_PRE_PADDING], toSend.length, LWS_WRITE_BINARY);

                    free(toSend.buffer);
                }
				// and schedule ourselves again
				usleep(1000);
				mcounterAudio++;
			}
			
			libwebsocket_callback_on_writable(this, wsi);
			
			break;
			
        default:
            break;
    }
   
   
    return 0;
}

static struct libwebsocket_protocols protocols[] = {
	/* first protocol must always be HTTP handler */

	{
		"http-only",		/* name */
		callback_http,		/* callback */
		2000000,	/* per_session_data_size */
		200000,			/* max frame size / rx buffer */
	},
	{
		"dumb-increment-protocol",
		callback_dumb_increment,
		6000000,
		200000,
	},
	{ NULL, NULL, 0, 0 } /* terminator */
};


void * serviceThread()
{
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

    while(workingPort == 0)
        usleep(100);
    
    if(workingPort == 4001 || workingPort == 4003) // For video sessions
        port= 9001;  
    else if(workingPort == 4002 || workingPort == 4004)
        port= 9002;   // New additional port
		
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
    
    {
        FILE *tempTrigger = fopen("socketServerReady.trig","w");
        fclose(tempTrigger);
    }

        while(1){
			libwebsocket_service(context, 50);
        }
		libwebsocket_context_destroy(context);

}

int calling_main(void) {

    pthread_t tid1;

    pthread_create(&tid1,NULL,serviceThread,NULL);
   
    printf("starting server...\n");
   
   
    return 0;
}






