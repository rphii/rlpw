#include <rlc/err.h>
#include <rlc/array.h>
#include <stdlib.h>
#include "pw.h"
#include "pw-task.h"
#include "pw-sched.h"
#include <unistd.h>

void *pw_task(void *void_pw_task) {
    ASSERT_ARG(void_pw_task);
    Pw_Task *task = void_pw_task;
    Pw_Queue *queue = &task->pw->queue;
    Pw_Sched *sched = &task->pw->sched;
    size_t len = SIZE_MAX;
    //printff("%2u started", task->id);
    while(!sched->cancel) {
        if(!len) {
            ///printff("%2u ready", task->id);
            pthread_mutex_lock(&sched->mutex);
            ++sched->ready;
            pthread_mutex_unlock(&sched->mutex);
            pthread_cond_wait(&sched->cond, &task->wait);
            pthread_mutex_lock(&sched->mutex);
            --sched->ready;
            pthread_mutex_unlock(&sched->mutex);
            ///printff("%2u woke up", task->id);
        }
        if(sched->cancel) break;
        pthread_mutex_lock(&queue->mutex);
        len = array_len(queue->data);
        if(len > 1) pthread_cond_signal(&sched->cond);
        Pw_User user = len > 0 ? array_pop(queue->data) : (Pw_User){0};
        pthread_mutex_unlock(&queue->mutex);
        if(len < 1) continue;
        --len;
        user.callback(user.data);
    }
    //printff("%2u quit", task->id);
    return 0;
}

