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
    Pw_Queue_Item *add = 0;
    NEW(Pw_Queue_Item, add);
    add->data = user;
    pthread_mutex_lock(&pw->queue.mutex);
    //printff(" Q ADD");
    if(pw->queue.last) {
        Pw_Queue_Item *last = pw->queue.last;
        //printff(" Q LAST = NEXT");
        last->next = add;
        pw->queue.last = add;
    } else {
        //printff(" Q LAST = ADD");
        pw->queue.last = add;
        pw->queue.next = add;
    }
    pthread_mutex_unlock(&pw->queue.mutex);
    pthread_cond_signal(&pw->sched.cond);
}

Pw_User pw_queue_pop_front(Pw *pw) {
    Pw_User result = {0};
    pthread_mutex_lock(&pw->queue.mutex);
    //printff(" Q POP");
    Pw_Queue_Item *next = pw->queue.next;
    if(next) {
        pw->queue.next = next->next;
        pthread_cond_signal(&pw->sched.cond);
        if(!next->next) {
            pw->queue.last = 0;
            //printff(" Q LAST = 0");
        }
    }
    pthread_mutex_unlock(&pw->queue.mutex);
    if(next) {
        result = next->data;
        free(next);
    }
    return result;
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


