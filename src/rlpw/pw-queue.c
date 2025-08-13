#include <rlc/err.h>
#include <rlc/array.h>
#include <stdlib.h>
#include "pw.h"
#include "pw-queue.h"
#include <string.h>

void pw_queue(Pw *pw, Pw_Callback callback, void *data) {
    ASSERT_ARG(pw);
    ASSERT_ARG(callback);
    Pw_User user = { .callback = callback, .data = data };
    pthread_mutex_lock(&pw->queue.mutex);
    array_push(pw->queue.data, user);
    pthread_mutex_unlock(&pw->queue.mutex);
    pthread_cond_signal(&pw->sched.cond);
}

void pw_when_done(Pw *pw, Pw_Callback callback, void *data) {
    ASSERT_ARG(pw);
    ASSERT_ARG(callback);
    Pw_User user = { .callback = callback, .data = data };
    pw->when_done = user;
}

void pw_when_done_clear(Pw *pw) {
    memset(&pw->when_done, 0, sizeof(pw->when_done));
}


