#ifndef BGW_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>

#include "bgw-task.h"
#include "bgw-sched.h"
#include "bgw-queue.h"

typedef struct Bgw {
    /* threads ::: individual threads loading files */
    Bgw_Tasks tasks;
    /* sched ::: schedule available tasks */
    Bgw_Sched sched;
    /* queue ::: queue holding user data */
    Bgw_Queue queue;
} Bgw;

void bgw_init(Bgw *bgw, uint jobs);
bool bgw_is_busy(Bgw *bgw);
void bgw_dispatch(Bgw *bgw);
void bgw_free(Bgw *bgw);
void bgw_queue(Bgw *bgw, Bgw_Callback callback, void *data);

#define BGW_H
#endif

