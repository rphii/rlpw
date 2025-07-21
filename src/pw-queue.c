#include <rl/err.h>
#include <rl/array.h>
#include <stdlib.h>
#include "pw.h"
#include "pw-queue.h"

void pw_queue(Pw *pw, Pw_Callback callback, void *data) {
    ASSERT_ARG(pw);
    ASSERT_ARG(callback);
    Pw_User user = { .callback = callback, .data = data };
    pthread_mutex_lock(&pw->queue.mutex);
    array_push(pw->queue.data, user);
    pthread_mutex_unlock(&pw->queue.mutex);
    pthread_cond_signal(&pw->sched.cond);
}


