#include "pw.h"
#include "pw-sched.h"

#include <stdlib.h>
#include <rlc/err.h>
#include <rlc/array.h>
#include <string.h>
#include <unistd.h>

void pw_init(Pw *pw, unsigned int jobs) {
    ASSERT_ARG(pw);
    ASSERT_ARG(jobs);
    pw->sched.jobs  = jobs;
    array_resize(pw->tasks, jobs);
}

bool pw_is_busy(Pw *pw) {
    pthread_mutex_t *unlock;
    bool busy = true;
    if(pw_queue_is_empty_lock_context(pw, &unlock)) {
        if(!pthread_mutex_trylock(&pw->sched.mutex)) {
            busy = pw->sched.ready != pw->sched.jobs;
            pthread_mutex_unlock(&pw->sched.mutex);
        }
        pthread_mutex_unlock(unlock);
    }
    return busy;
}

int pw_dispatch(Pw *pw) {
    int err = 0;
    for(size_t i = 0; !err && i < pw->sched.jobs; ++i) {
        Pw_Task *task = array_it(pw->tasks, i);
        task->pw = pw;
        task->id = i;
        err = pthread_create(&task->thread, 0, pw_task, task);
        //usleep(1000);
    }
    return err;
}

void pw_cancel(Pw *pw) {
    pw->sched.cancel = true;
    pthread_mutex_lock(&pw->sched.wait);
    for(unsigned int i = 0; i < pw->sched.jobs; ++i) {
        pthread_cond_signal(&pw->sched.cond);
    }
    pthread_mutex_unlock(&pw->sched.wait);
}

void pw_free(Pw *pw) {
    pw_cancel(pw);
    for(unsigned int i = 0; i < pw->sched.jobs; ++i) {
        Pw_Task *task = array_it(pw->tasks, i);
        pthread_join(task->thread, 0);
    }
    array_free(pw->queue);
    array_free(pw->tasks);
    memset(pw, 0, sizeof(*pw));
}

