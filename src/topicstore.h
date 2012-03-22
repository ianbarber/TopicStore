#ifndef _topicstore_h
#define _topicstore_h

#include <zmq.h>
#include <time.h>

#define TOPS_MAX_TOPIC_SIZE 512
#define TOPS_MAX_DSN_SIZE 256
#define TOPS_MAX_TYPE_SIZE 4
#define TOPS_MAX_HEADER_SIZE 20
#define TOPS_MAX_EXPIRY_SIZE 12
#define TOPS_OK "TOPS01_OK"
#define TOPS_ADD "TOPS01_ADD"
#define TOPS_REM "TOPS01_REM"
#define TOPS_GET "TOPS01_GET"
#define TOPS_TOP "TOPS01_TOP"
#define TOPS_WAT "TOPS01_WAT"

typedef struct {
    char type[TOPS_MAX_TYPE_SIZE];
    char dsn[TOPS_MAX_DSN_SIZE];
	time_t expiry;
    void *next;
} tops_topic_msg;

#endif