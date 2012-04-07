#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "topicstore.h"
#include "trie.h"

/* 
	TODO: Compact trie?
	TODO: add a counter of nodes or similar to estimate tree size
*/

tops_topic_msg *trie_expire_values (tops_topic_msg *value) {
	tops_topic_msg *root, *prev, *next;
	time_t curtime = time(NULL);
	root = value;
	prev = next = NULL;
	
	do {
		next = value->next;
		if (value->expiry <= curtime) {
			if (value == root) {
				root = next;
			} else if (prev != NULL) {
				prev->next = next;
			}
			free(value);
		}  else {
			prev = value;
		}
	} while (next != NULL && (value = next));
	
	return root;
}

trienode_t *trie_find_node (trienode_t *trie, char *topic) {
	trienode_t *curtrie;
	int index;
	
	index = 0;
	curtrie = trie;
	
	while (index < TOPS_MAX_TOPIC_SIZE) {
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
		
		/* If we're on the string terminator, exit */
		if (topic[index] == '\0' ) {
			break;
		}
	}
	
	return curtrie;
}


/* Copy a selection of nodes into new memory for returning */
tops_topic_msg *trie_walk_values (trienode_t *trie) {
	tops_topic_msg *retvals, *curvals, *walkvals, *tmsg;
	
	curvals = NULL;
	
	do {		
		/* Pull out the current node value set, if any */
		if (trie->value != NULL) {
			/* Expire node sets */
			trie->value = trie_expire_values (trie->value);
			walkvals = trie->value;
			
			/* Walk the list of values */
			do {
				/* We don't care about some values, so no memcpy */
				tmsg = malloc (sizeof (tops_topic_msg));
				memcpy(tmsg->type, walkvals->type, strlen(walkvals->type) + 1);
				memcpy(tmsg->dsn, walkvals->dsn, strlen(walkvals->dsn) + 1);
				tmsg->next = NULL;

				if (curvals == NULL) {
					curvals = retvals = tmsg;
				} else {
					curvals->next = tmsg;
					curvals = curvals->next;
				}
			} while (walkvals->next != NULL && (walkvals = walkvals->next));
		}
		
		/* Recurse into trie */
		if (trie->children != NULL) {
			if (curvals == NULL) {
				curvals = retvals = trie_walk_values (trie->children);
			} else {
				curvals->next = trie_walk_values (trie->children);
			}
			
			/* iterate to end of linked list */
			while (curvals->next != NULL && (curvals = curvals->next)) {}
		}
	} while (trie->next != NULL && (trie = trie->next));
	
	return retvals;
}

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
		
		/* If we're on the string terminator, exit */
		if (topic[index] == '\0' ) {
			break;
		}
		
		index++;
	}
	
	/* We should now be at a null entry for a topic with a value. */
	if(curtrie->value == NULL) {
		/* If there is no record here, we can just insert one */
		curtrie->value = tmsg;
	} else {
		/* Iterate over the list of values, insert at end
		unless the item is already there - if so just update the 
		expiry time and leave it at that  */
		
		tempmsg = curtrie->value;
		do {
			if (strcmp (tempmsg->dsn, tmsg->dsn) == 0 ) {
				tempmsg->expiry = tmsg->expiry;
				free (tmsg);
				/* Update expiry, return now */
				return 0;
			}
		} while( tempmsg->next != NULL );
		
		tempmsg->next = tmsg;
	}
	
	return 0;
}

tops_topic_msg *trie_get_dsns (trienode_t *trie, char *topic) {
	trienode_t *curtrie;
	tops_topic_msg *retval;

	retval = NULL;
	curtrie = trie_find_node (trie, topic);
	
	if (curtrie != NULL) {
		/* 
		So we have a matching topic - everything under
		here definitely matches our pattern, so we need to walk
		the trie and values to get a list of DSNs to return. 
		*/
		retval = trie_walk_values (curtrie->children);
	}
	
	return retval;
}

int trie_rem_dsn (trienode_t *trie, char *topic, char *dsn) {
	trienode_t *curtrie;
	tops_topic_msg *tmsg, *previous;
	int rc = 0; 
	previous = NULL;
	
	curtrie = trie_find_node (trie, topic);
	
	curtrie = curtrie->children;
	
	while(curtrie != NULL && curtrie->key != '\0') {
		curtrie = curtrie->next;
	}
	
	if (curtrie != NULL && curtrie->value) {
		/* Walk the list of values */
		tmsg = curtrie->value;
		do {
			if ( strcmp (tmsg->dsn, dsn) == 0 ) {
				if (previous) {
					previous->next = tmsg->next;
				} else {
					curtrie->value = tmsg->next;
				}
			}

			previous = tmsg;
		} while (tmsg->next != NULL && (tmsg = tmsg->next));
	}
	
	return rc;
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