#include "rlpw.h"
#include <rlc/err.h>
#include <rlso.h>
#include <unistd.h>

typedef enum {
    TASK_FILE,
    TASK_WALK,
} Task_List;

typedef struct Task {
    Task_List id;
    Pw *pw;
    So file_or_dir;
    VSo find;
} Task;

pthread_mutex_t m;

int queue_file(So filename, void *void_task);
int queue_walk(So dir, void *void_task);

#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void *task(void *void_task) {
    Task *task = void_task;
    switch(task->id) {
        case TASK_FILE: {
            size_t len = array_len(task->find);
            if(!len) {
                pthread_mutex_lock(&m);
                printf("%.*s\n", SO_F(task->file_or_dir));
                pthread_mutex_unlock(&m);
            } else {
                for(size_t i = 0; i < len; ++i) {
                    So find = array_at(task->find, i);
                    size_t x = so_find_sub(task->file_or_dir, find, true);
                    if(x < so_len(task->file_or_dir)) {
                        pthread_mutex_lock(&m);
                        printf("%.*s\n", SO_F(task->file_or_dir));
                        pthread_mutex_unlock(&m);
                    } else {
                    }
                }
            }
        } break;
        case TASK_WALK: {
            //printf("w %.*s\n", SO_F(task->file_or_dir));
            so_file_exec(task->file_or_dir, true, true, queue_file, queue_walk, task);
        } break;
    }
    free(void_task);
    return 0;
}


int queue_walk(So filename, void *void_task) {
    Task *t = void_task;
    Task *q = malloc(sizeof(*q));
    memcpy(q, t, sizeof(*q));
    q->id = TASK_WALK;
    q->file_or_dir = filename;
    pthread_mutex_lock(&m);
    //printf("WALK %.*s\n", SO_F(q->file_or_dir));
    //getchar();
    pthread_mutex_unlock(&m);
    pw_queue(q->pw, task, q);
    //printf("WALK NEXT\n");
    return 0;
}

int queue_file(So filename, void *void_task) {
    Task *t = void_task;
    Task *q = malloc(sizeof(*q));
    memcpy(q, t, sizeof(*q));
    q->id = TASK_FILE;
    q->file_or_dir = filename;
    pthread_mutex_lock(&m);
    //printf("FILE %.*s\n", SO_F(q->file_or_dir));
    pthread_mutex_unlock(&m);
    pw_queue(q->pw, task, q);
    //printf("FILE NEXT\n");
    return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>


int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    if(!getcwd(cwd, sizeof(cwd))) return -1;
    //printf("cwd:%.*s\n", SO_F(str_l(cwd)));
    Pw pw = {0};
    pw_init(&pw, 16);
    pw_dispatch(&pw);
    Task task = {
        .pw = &pw,
    };
    for(int i = 1; i < argc; ++i) {
        array_push(task.find, so_l(argv[i]));
    }
    queue_walk(so_l(cwd), &task);
    //while(true) { usleep(5000000); printf("sleep..\n"); }
    while(pw_is_busy(&pw)) { usleep(1000); }
    pw_free(&pw);
    array_free(task.find);
    //printff("done");
    return 0;
}

