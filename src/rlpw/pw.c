#include "pw.h"
#include "pw-sched.h"

#include <stdlib.h>
#include <rlc/err.h>
#include <rlc/array.h>
#include <string.h>
#include <unistd.h>

void pw_init(Pw *pw, uint jobs) {
    ASSERT_ARG(pw);
    ASSERT_ARG(jobs);
    pw->sched.jobs  = jobs;
    array_resize(pw->tasks, jobs);
}

bool pw_is_busy(Pw *pw) {
    Pw_Queue_Item *next = 0;
    uint ready;
    if(!pthread_mutex_trylock(&pw->queue.mutex)) {
        next = pw->queue.next;
        pthread_mutex_unlock(&pw->queue.mutex);
    } else {
        return true;
    }
    if(!pthread_mutex_trylock(&pw->sched.mutex)) {
        ready = pw->sched.ready;
        pthread_mutex_unlock(&pw->sched.mutex);
    } else {
        return true;
    }
    return (next != 0) || (ready != pw->sched.jobs);
}

void pw_dispatch(Pw *pw) {
    int err = 0;
    for(size_t i = 0; i < pw->sched.jobs; ++i) {
        Pw_Task *task = array_it(pw->tasks, i);
        task->pw = pw;
        task->id = i;
        err = pthread_create(&task->thread, 0, pw_task, task);
        usleep(1000);
    }
}

void pw_cancel(Pw *pw) {
    pw->sched.cancel = true;
    pthread_mutex_lock(&pw->sched.wait);
    for(uint i = 0; i < pw->sched.jobs; ++i) {
        pthread_cond_signal(&pw->sched.cond);
    }
    pthread_mutex_unlock(&pw->sched.wait);
}

void pw_free(Pw *pw) {
    pw_cancel(pw);
    for(uint i = 0; i < pw->sched.jobs; ++i) {
        Pw_Task *task = array_it(pw->tasks, i);
        pthread_join(task->thread, 0);
    }
    array_free(pw->queue);
    array_free(pw->tasks);
    memset(pw, 0, sizeof(*pw));
}

