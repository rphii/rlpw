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
    bool have_more = false;
    //printff("%2u started", task->id);
    while(!sched->cancel) {
        pthread_mutex_lock(&sched->mutex);
        have_more = queue->next;
        pthread_mutex_unlock(&sched->mutex);
        if(!have_more) {
            pthread_mutex_lock(&sched->mutex);
            //printff("%2u ready", task->id);
            if(sched->ready + 1 == sched->jobs) {
                if(!pthread_mutex_trylock(&queue->mutex)) {
                    bool callback = !queue->next;
                    pthread_mutex_unlock(&queue->mutex);
                    if(callback && when_done->callback) when_done->callback(when_done->data);
                }
            }
            ++sched->ready;
            //printff("%2u wait",task->id);
            pthread_mutex_unlock(&sched->mutex);
            pthread_cond_wait(&sched->cond, &task->wait);
            pthread_mutex_lock(&sched->mutex);
            --sched->ready;
            //printff("%2u woke up", task->id);
            pthread_mutex_unlock(&sched->mutex);
        }
        if(sched->cancel) break;
        user = pw_queue_pop_front(task->pw);
        if(user.callback) {
            //printff("%2u work",task->id);
            user.callback(user.data);
        }
    }
    //printff("%2u quit", task->id);
    return 0;
}

