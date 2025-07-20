#ifndef BGW_QUEUE_H

#include <pthread.h>
#include "bgw-user.h"

typedef struct Bgw Bgw;

typedef struct Bgw_Queue {
    Bgw_User *data;
    pthread_mutex_t mutex;
} Bgw_Queue;

void bgw_queue(Bgw *bgw, Bgw_Callback callback, void *data);

#define BGW_QUEUE_H
#endif

