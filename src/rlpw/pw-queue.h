#ifndef BGW_QUEUE_H

#include <pthread.h>
#include "pw-user.h"

typedef struct Pw Pw;

typedef struct Pw_Queue {
    Pw_User *data;
    pthread_mutex_t mutex;
} Pw_Queue;

void pw_queue(Pw *pw, Pw_Callback callback, void *data);

#define BGW_QUEUE_H
#endif

