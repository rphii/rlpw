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
    Pw_User *when_done = &task->pw->when_done;
    Pw_User user = {0};
    pthread_mutex_t *unlock;
    //printff("%2u started", task->id);
    while(!sched->cancel) {
        if(pw_queue_is_empty_lock_context(task->pw, &unlock)) {
            pthread_mutex_lock(&sched->mutex);
            bool do_callback = ++sched->ready == sched->jobs && when_done->callback && !queue->next;
            //printff("%2u wait",task->id);
            pthread_mutex_unlock(&sched->mutex);
            pthread_mutex_unlock(unlock);
            if(do_callback) {
                when_done->callback(task->pw, &sched->cancel, when_done->data);
            } else {
                pthread_cond_wait(&sched->cond, &task->wait);
            }
            pthread_mutex_lock(&sched->mutex);
            --sched->ready;
            //printff("%2u woke up", task->id);
            pthread_mutex_unlock(&sched->mutex);
        }
        if(sched->cancel) break;
        user = pw_queue_pop_front(task->pw);
        if(user.callback) {
            //printff("%2u work",task->id);
            user.callback(task->pw, &sched->cancel, user.data);
        }
    }
    //printff("%2u quit", task->id);
    return 0;
}

