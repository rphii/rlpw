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

bool bgw_is_busy(Bgw *bgw);

void bgw_init(Bgw *bgw, uint jobs);
void bgw_dispatch(Bgw *bgw);
void bgw_cancel(Bgw *bgw);
void bgw_free(Bgw *bgw);

#define BGW_H
#endif

