#ifndef RLPW_SCHED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include "pw-task.h"

typedef struct Pw_Sched {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_mutex_t wait;
    bool cancel;
    bool initialized;
    unsigned int jobs;
    unsigned int ready;
} Pw_Sched;

#define RLPW_SCHED_H
#endif

