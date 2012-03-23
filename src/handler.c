#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "trie.h"
#include "handler.h"

// Should probably do this with a macro to be more C-like
void handler_nullcheck (char *string, int length) {
	string[length] = '\0';
}

handler_t *handler_init () {
	handler_t *handler;
	handler = malloc (sizeof (trienode_t));
	handler->trieroot = trie_init();
	return handler;
}

// TODO: Add error handling
int handler_add (handler_t *handler, void *socket, void *pub) {
	// TODO: Document size limit in spec
	int rc, more, len;
	size_t size;
	time_t expiry;
	char type [TOPS_MAX_TYPE_SIZE];
	char topic [TOPS_MAX_TOPIC_SIZE];
	char dsn [TOPS_MAX_DSN_SIZE];
	char expirystring [TOPS_MAX_EXPIRY_SIZE];
	
	// TODO: Check multipart flag!
	// TODO: Check sizes
	len = zmq_recv (socket, type, TOPS_MAX_TYPE_SIZE, 0);
	handler_nullcheck(type, len);
	len = zmq_recv (socket, topic, TOPS_MAX_TOPIC_SIZE, 0);
	handler_nullcheck(topic, len);
	// TODO: should this be the time in bytes rather than chars?
	len = zmq_recv (socket, expirystring, TOPS_MAX_EXPIRY_SIZE, 0);
	handler_nullcheck(expirystring, len);
	
	size = sizeof(int);
	rc = zmq_send (pub, topic, strlen(topic), ZMQ_SNDMORE);
	
	// TODO: error checking
	// TODO: Handle invalid number of message parts better.
	zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &size);
	while ( more == 1 ) {
		len = zmq_recv (socket, dsn, TOPS_MAX_DSN_SIZE, 0);
		handler_nullcheck(dsn, len);
		zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &size);
		rc = zmq_send (pub, topic, strlen(topic), more == 1 ? ZMQ_SNDMORE : 0);
		rc = trie_add_dsn (handler->trieroot, topic, type, dsn, expiry);
	}
	
	// TODO: Modify expiry time by maximum server value rather than just client
	rc = zmq_send (socket, TOPS_OK, strlen(TOPS_OK), ZMQ_SNDMORE);
	rc = zmq_send (socket, expirystring, strlen(expirystring), 0);
	
	return 0;
}

int handler_get (handler_t *handler, void *socket) {
	int rc, len;
	char topic [TOPS_MAX_TOPIC_SIZE];
	tops_topic_msg *dsns, *last;
	
	len = zmq_recv (socket, topic, TOPS_MAX_TOPIC_SIZE, 0);
	handler_nullcheck(topic, len);
	dsns = trie_get_dsns (handler->trieroot, topic);
	
	rc = zmq_send (socket, TOPS_TOP, strlen(TOPS_TOP), dsns == NULL ? 0 : ZMQ_SNDMORE );
	if (dsns == NULL) {
		return 0; // not found
	}
	
	rc = zmq_send (socket, dsns->type, strlen(dsns->type), ZMQ_SNDMORE);
	while (dsns->next != NULL) {
		rc = zmq_send (socket, dsns->dsn, strlen(dsns->dsn), ZMQ_SNDMORE);
		last = dsns;
		dsns = dsns->next;
		// TODO: remove these frees? Should be part of the structure the tree manages
		free(last);
	}
	rc = zmq_send (socket, dsns->dsn, strlen(dsns->dsn), 0);
	free(dsns);
	
	return 0;
}


// TODO: remove!
int handler_rem (handler_t *handler, void *socket) {
	return 0;
}

int handler_unknown (handler_t *handler, void *socket) {
	int rc, more;
	size_t size;
	char topic [TOPS_MAX_TOPIC_SIZE];
	
	// Drain all message parts
	while ( zmq_getsockopt (socket, ZMQ_RCVMORE, &more, &size) == 0 && more == 1 ) {
		rc = zmq_recv (socket, topic, TOPS_MAX_TOPIC_SIZE, 0);
	}
	
	// Send unknown message response
	return zmq_send (socket, TOPS_WAT, strlen(TOPS_WAT), 0);
}

void handler_close (handler_t *handler) {
	// TODO: shut down the trie 
	return;
}