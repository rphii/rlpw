#include "bgw.h"
#include <rphii/err.h>
#include <rphii/str.h>
#include <rphii/file.h>
#include <unistd.h>

typedef enum {
    TASK_FILE,
    TASK_WALK,
} Task_List;

typedef struct Task {
    Task_List id;
    Bgw *bgw;
    Str file_or_dir;
    VStr find;
} Task;

pthread_mutex_t m;

int queue_file(Str filename, void *void_task);
int queue_walk(Str dir, void *void_task);

#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

ErrDecl file_exec2(Str path, bool recursive, bool hidden, FileFunc exec, FileFunc xdir, void *args) {
    ASSERT_ARG(exec);
    //printf("path: %.*s\n", STR_F(path));
    int err = 0;
    DIR *dir = 0;
    Str dot = str(".");
    Str dotdot = str("..");
    if(!str_len_raw(path)) return 0;
    FileTypeList type = file_get_type(path);
    Str filename = {0};
    if(type == FILE_TYPE_DIR) {
        if(!recursive) {
            THROW("will not go over '%.*s' (enable recursion to do so)", STR_F(path));
        }
        struct dirent *dp = 0;
        Str direns = str_ensure_dir(path);
        char cdir[FILE_PATH_MAX];
        str_as_cstr(direns, cdir, FILE_PATH_MAX);
        if((dir = opendir(cdir)) == NULL) {
            goto clean;
            THROW("can't open directory '%.*s'", STR_F(direns));
        }
        while((dp = readdir(dir)) != NULL) {
            Str dname = str_l(dp->d_name);
            if(!str_cmp(dname, dot)) continue;
            if(!str_cmp(dname, dotdot)) continue;
            if(!hidden && !str_cmp0(dname, dot)) continue;
            str_extend(&filename, direns);
            if(str_len_raw(direns) > 1) str_push(&filename, PLATFORM_CH_SUBDIR);
            str_extend(&filename, dname);
            if(!str_len_raw(filename)) {
                str_clear(&filename);
                continue;
            }
            FileTypeList type2 = file_get_type(filename);
            if(type2 == FILE_TYPE_DIR) {
                xdir(filename, args);
                str_zero(&filename);
            } else if(type2 == FILE_TYPE_FILE) {
                exec(filename, args);
                str_zero(&filename);
                //str_clear(&filename);
            } else {
                str_clear(&filename);
                //info(INFO_skipping_nofile_nodir, "skipping '%.*s' since no regular file nor directory", STR_F(*path));
            }
        }
    } else if(type == FILE_TYPE_FILE) {
        TRY(exec(path, args), "an error occured while executing the function");
    } else if(type == FILE_TYPE_ERROR) {
        THROW("failed checking type of '%.*s' (maybe it doesn't exist?)", STR_F(path));
    } else {
        //info(INFO_skipping_nofile_nodir, "skipping '%.*s' since no regular file nor directory", STR_F(*path));
    }
clean:
    str_free(&filename);
    if(dir) closedir(dir);
    return err;
error: ERR_CLEAN;
}


void *task(void *void_task) {
    Task *task = void_task;
    switch(task->id) {
        case TASK_FILE: {
            size_t len = array_len(task->find);
            if(!len) {
                pthread_mutex_lock(&m);
                printf("%.*s\n", STR_F(task->file_or_dir));
                pthread_mutex_unlock(&m);
            } else {
                for(size_t i = 0; i < len; ++i) {
                    Str find = array_at(task->find, i);
                    size_t x = str_find_substr(task->file_or_dir, find, true);
                    if(x < str_len_raw(task->file_or_dir)) {
                        pthread_mutex_lock(&m);
                        printf("%.*s\n", STR_F(task->file_or_dir));
                        pthread_mutex_unlock(&m);
                    } else {
                    }
                }
            }
        } break;
        case TASK_WALK: {
            file_exec2(task->file_or_dir, true, true, queue_file, queue_walk, task);
        } break;
    }
    free(void_task);
    return 0;
}


int queue_walk(Str filename, void *void_task) {
    Task *t = void_task;
    Task *q = malloc(sizeof(*q));
    memcpy(q, t, sizeof(*q));
    q->id = TASK_WALK;
    q->file_or_dir = filename;
    bgw_queue(q->bgw, task, q);
    return 0;
}

int queue_file(Str filename, void *void_task) {
    Task *t = void_task;
    Task *q = malloc(sizeof(*q));
    memcpy(q, t, sizeof(*q));
    q->id = TASK_FILE;
    q->file_or_dir = filename;
    bgw_queue(q->bgw, task, q);
    return 0;
}

#include <unistd.h>
#include <stdio.h>
#include <linux/limits.h>


int main(int argc, char **argv) {
    char cwd[PATH_MAX];
    if(!getcwd(cwd, sizeof(cwd))) return -1;
    //printf("cwd:%.*s\n", STR_F(str_l(cwd)));
    Bgw bgw = {0};
    bgw_init(&bgw, 16);
    bgw_dispatch(&bgw);
    Task task = {
        .bgw = &bgw,
    };
    for(int i = 1; i < argc; ++i) {
        array_push(task.find, str_l(argv[i]));
    }
    queue_walk(str_l(cwd), &task);
    while(bgw_is_busy(&bgw)) { usleep(1000); }
    bgw_free(&bgw);
    array_free(task.find);
    //printff("done");
    return 0;
}

