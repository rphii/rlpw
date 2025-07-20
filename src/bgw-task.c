#include <rphii/err.h>
#include <rphii/array.h>
#include <stdlib.h>
#include "bgw.h"
#include "bgw-task.h"
#include "bgw-sched.h"
#include <unistd.h>

void *bgw_task(void *void_bgw_task) {
    ASSERT_ARG(void_bgw_task);
    Bgw_Task *task = void_bgw_task;
    Bgw_Queue *queue = &task->bgw->queue;
    Bgw_Sched *sched = &task->bgw->sched;
    size_t len = SIZE_MAX;
    //printff("%2u started", task->id);
    while(!sched->cancel) {
        if(!len) {
            ///printff("%2u ready", task->id);
            pthread_mutex_lock(&sched->mutex);
            ++sched->available.count;
            pthread_mutex_unlock(&sched->mutex);
            pthread_cond_wait(&sched->cond, &task->wait);
            pthread_mutex_lock(&sched->mutex);
            --sched->available.count;
            pthread_mutex_unlock(&sched->mutex);
            ///printff("%2u woke up", task->id);
        }
        if(sched->cancel) break;
        pthread_mutex_lock(&queue->mutex);
        len = array_len(queue->data);
        if(len > 1) pthread_cond_signal(&sched->cond);
        Bgw_User user = len > 0 ? array_pop(queue->data) : (Bgw_User){0};
        pthread_mutex_unlock(&queue->mutex);
        if(len < 1) continue;
        --len;
        user.callback(user.data);
    }
    //printff("%2u quit", task->id);
    return 0;
}

