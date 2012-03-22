#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "topicstore.h"
#include "trie.h"

// TODO: FS sync?
// TODO: delete 
// TODO: Compact trie?

trienode_t *trie_init () {
	trienode_t *trie;
	trie = malloc (sizeof (trienode_t));
	trie->next = NULL;
	trie->children = NULL;
	trie->value = NULL;
	return trie;
}

int trie_add_dsn (trienode_t *trie, char *topic, char *type, char *dsn, time_t expiry) {
	tops_topic_msg *tmsg, *tempmsg;
	trienode_t *curtrie;
	int index;
	
	tmsg = trie_build_record (type, dsn, expiry);
	curtrie = trie;
	index = 0;
	
	while (index < TOPS_MAX_TOPIC_SIZE) {
		if (curtrie->children == NULL) {
			curtrie->children = trie_init();
			curtrie->children->key = topic[index];
		}
		
		curtrie = curtrie->children;
		
		while ( curtrie->next != NULL ) {
			if (curtrie->key == topic[index]) {
				break;
			} else {
				curtrie = curtrie->next;
			}
		}
		
		if( curtrie->key != topic[index] ) {
			curtrie->next = trie_init();
			curtrie->next->key = topic[index];
			curtrie = curtrie->next;
		}
		
		// If we're on the string terminator, exit 
		if (topic[index] == '\0' ) {
			break;
		}
		
		index++;
	}
	
	// We should now be at a null entry for a topic with a value.
	if(curtrie->value == NULL) {
		// If there is no record here, we can just insert one
		curtrie->value = tmsg;
	} else {
		// Iterate over the list of values, insert 
		tempmsg = curtrie->value;
		while( tempmsg->next != NULL ) {
			tempmsg = tempmsg->next;
		}
		tempmsg->next = tmsg;
	}
	
	return 0;
}

tops_topic_msg *trie_get_dsns (trienode_t *trie, char *topic) {
	trienode_t *curtrie;
	int index;
	
	index = 0;
	curtrie = trie;
	
	while (topic[index] != '\0') {
		if (curtrie->children == NULL) {
			curtrie = NULL;
			break;
		}
		
		curtrie = curtrie->children;
		
		while ( curtrie->next != NULL ) {
			if (curtrie->key == topic[index]) {
				break;
			} else {
				curtrie = curtrie->next;
			}
		}
		
		if( curtrie->key != topic[index] ) {
			curtrie = NULL;
			break;
		}
		
		index++;
	}
	
	// TODO: We now have our matching topic trie. We need to pull out a list
	// of DSNS by walking the subtree
	
	return curtrie == NULL ? NULL : curtrie->value;
	
}

tops_topic_msg *trie_build_record (char *type, char *dsn, time_t expiry) {
	tops_topic_msg *tmsg;
	
	tmsg = malloc (sizeof (tops_topic_msg));
	snprintf(tmsg->type, TOPS_MAX_TOPIC_SIZE, "%s", type);
	snprintf(tmsg->dsn, TOPS_MAX_DSN_SIZE, "%s", dsn);
	tmsg->expiry = expiry;
	tmsg->next = NULL;
	
	return tmsg;
}