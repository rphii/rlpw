#include <rphii/err.h>
#include <rphii/array.h>
#include <stdlib.h>
#include "bgw.h"
#include "bgw-queue.h"

void bgw_queue(Bgw *bgw, Bgw_Callback callback, void *data) {
    ASSERT_ARG(bgw);
    ASSERT_ARG(callback);
    Bgw_User user = { .callback = callback, .data = data };
    pthread_mutex_lock(&bgw->queue.mutex);
    array_push(bgw->queue.data, user);
    pthread_mutex_unlock(&bgw->queue.mutex);
    pthread_cond_signal(&bgw->sched.cond);
}


