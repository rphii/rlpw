#ifndef BGW_SCHED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include "bgw-task.h"

typedef struct Bgw_Sched {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_t wait;
    bool cancel;
    bool initialized;
    uint jobs;
    struct {
        uint count;
        uint now;
        uint end;
    } available;
    Bgw_Task **next;
} Bgw_Sched;

#define BGW_SCHED_H
#endif

