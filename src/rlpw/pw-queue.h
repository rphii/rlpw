#ifndef BGW_QUEUE_H

#include <pthread.h>
#include "pw-user.h"

typedef struct Pw Pw;

typedef struct Pw_Queue_Item {
    Pw_User data;
    struct Pw_Queue_Item *next;
} Pw_Queue_Item;

typedef struct Pw_Queue {
    Pw_Queue_Item *next;
    Pw_Queue_Item *last;
    pthread_mutex_t mutex;
} Pw_Queue;

bool pw_queue_is_empty_lock_context(Pw *pw, pthread_mutex_t **unlock);
void pw_queue(Pw *pw, Pw_Callback callback, void *data);
void pw_queue_front(Pw *pw, Pw_Callback callback, void *data);
Pw_User pw_queue_pop_front(Pw *pw);
void pw_when_done(Pw *pw, Pw_Callback callback, void *data);
void pw_when_done_clear(Pw *pw);

#define BGW_QUEUE_H
#endif

