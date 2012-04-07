#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "topicstore.h"
#include "handler.h"

/* 
	TODO: Signal handling
	TODO: Command line options
	TODO: Error output
	TODO: Max store size?
	TODO: Disk persistence for store?
	TODO: Flush any extraneous message parts that aren't expected
*/

int main(int argc, char *argv[])
{
	void *ctx, *socket, *pub;
	handler_t *handler;
	int rc; 
	char header[TOPS_MAX_HEADER_SIZE];
	
	ctx = zmq_init (1);
	// TODO: cmd line options for socket ports
	socket = zmq_socket (ctx, ZMQ_REP);
	rc = zmq_bind (socket, "tcp://*:9292");
	pub = zmq_socket (ctx, ZMQ_PUB);
	rc = zmq_bind  (pub, "tcp://*:9293");
	handler = handler_init ();
	
	while (1) {
		memset (header, 0, TOPS_MAX_HEADER_SIZE);
		zmq_recv (socket, header, TOPS_MAX_HEADER_SIZE, 0);
		//printf("DEBUG: %s\n", header);
		
		if (strcmp (header, TOPS_ADD) == 0) {
			rc = handler_add (handler, socket, pub);
		} else if (strcmp (header, TOPS_REM) == 0) {
			rc = handler_rem (handler, socket);
		} else if (strcmp (header, TOPS_GET) == 0) {
			rc = handler_get (handler, socket);
		} else {
			rc = handler_unknown (handler, socket);
		}
		
		if (rc != 0) {
			// TODO: asplode!
		}
	}
	handler_close (handler);
	zmq_close (socket);
	zmq_term (ctx);
	
	return 0;
}