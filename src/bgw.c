#include "bgw.h"
#include "bgw-sched.h"

#include <stdlib.h>
#include <rphii/err.h>
#include <rphii/array.h>
#include <string.h>
#include <unistd.h>

void bgw_init(Bgw *bgw, uint jobs) {
    ASSERT_ARG(bgw);
    ASSERT_ARG(jobs);
    bgw->sched.jobs = jobs = jobs;
    array_resize(bgw->tasks, jobs);
}

bool bgw_is_busy(Bgw *bgw) {
    pthread_mutex_lock(&bgw->queue.mutex);
    size_t len = array_len(bgw->queue.data);
    pthread_mutex_unlock(&bgw->queue.mutex);
    pthread_mutex_lock(&bgw->sched.mutex);
    uint av = bgw->sched.available.count;
    pthread_mutex_unlock(&bgw->sched.mutex);
    //printff("available %u", av);
    return len || !(av == bgw->sched.jobs);
    //return len;
}

void bgw_dispatch(Bgw *bgw) {
    int err = 0;
    for(size_t i = 0; i < bgw->sched.jobs; ++i) {
        Bgw_Task *task = array_it(bgw->tasks, i);
        task->bgw = bgw;
        task->id = i;
        err = pthread_create(&task->thread, 0, bgw_task, task);
        usleep(1000);
    }
}

void bgw_free(Bgw *bgw) {
    //printff("BGW FREE");
    bgw->sched.cancel = true;
    //printff("COND");
    pthread_mutex_lock(&bgw->sched.wait);
    for(uint i = 0; i < bgw->sched.jobs; ++i) {
        pthread_cond_signal(&bgw->sched.cond);
    }
    pthread_mutex_unlock(&bgw->sched.wait);
    //printff("JOIN");
    for(uint i = 0; i < bgw->sched.jobs; ++i) {
        Bgw_Task *task = array_it(bgw->tasks, i);
        pthread_join(task->thread, 0);
    }
    array_free(bgw->queue);
    array_free(bgw->tasks);
    memset(bgw, 0, sizeof(*bgw));
}

void bgw_queue(Bgw *bgw, Bgw_Callback callback, void *data) {
    ASSERT_ARG(bgw);
    ASSERT_ARG(callback);
    Bgw_User user = { .callback = callback, .data = data };
    pthread_mutex_lock(&bgw->queue.mutex);
    array_push(bgw->queue.data, user);
    pthread_mutex_unlock(&bgw->queue.mutex);
    pthread_cond_signal(&bgw->sched.cond);
}

