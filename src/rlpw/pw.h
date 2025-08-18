#ifndef RLPW_PW_H

#include <stddef.h>
#include <pthread.h>
#include <stdbool.h>

#include "pw-task.h"
#include "pw-sched.h"
#include "pw-queue.h"

typedef struct Pw {
    /* threads ::: individual threads loading files */
    Pw_Tasks tasks;
    /* sched ::: schedule available tasks */
    Pw_Sched sched;
    /* queue ::: queue holding user data */
    Pw_Queue queue;
    /* when_done ::: called when queue is emptied */
    Pw_User when_done;
} Pw;

bool pw_is_busy(Pw *pw);

void pw_init(Pw *pw, uint jobs);
int pw_dispatch(Pw *pw);
void pw_cancel(Pw *pw);
void pw_free(Pw *pw);

#define RLPW_PW_H
#endif

