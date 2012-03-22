#ifndef _handler_h
#define _handler_h

typedef struct handler {
	struct trienode *trieroot;
} handler_t;

handler_t *handler_init ();
int handler_add (handler_t *handler, void *socket, void *pub);
int handler_get (handler_t *handler, void *socket);
int handler_rem (handler_t *handler, void *socket);
int handler_unknown (handler_t *handler, void *socket);
void handler_close (handler_t *close);

#endif