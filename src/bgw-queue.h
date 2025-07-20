#ifndef BGW_QUEUE_H

#include <pthread.h>
#include "bgw-user.h"

typedef struct Bgw_Queue {
    Bgw_User *data;
    pthread_mutex_t mutex;
} Bgw_Queue;

#define BGW_QUEUE_H
#endif

