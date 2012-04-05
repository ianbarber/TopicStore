#ifndef _trie_h
#define _trie_h

#include "topicstore.h"

typedef struct trienode {
	char key;
	tops_topic_msg *value;
	struct trienode *children;
	struct trienode *next;
} trienode_t;

trienode_t *trie_init ();
int trie_add_dsn (trienode_t *trie, char *topic, char *type, char *dsn, time_t expiry);
tops_topic_msg *trie_get_dsns (trienode_t *trie, char *topic);
int trie_rem_dsn (trienode_t *trie, char *topic, char *dsn);
tops_topic_msg *trie_build_record (char *type, char *dsn, time_t expiry);

#endif