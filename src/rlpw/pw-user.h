#ifndef RLPW_USER_H

#include <stdbool.h>

typedef struct Pw Pw;

typedef void *(*Pw_Callback)(Pw *pw, bool *cancel, void *user, int pw_nb);

typedef struct Pw_User {
    Pw_Callback callback;
    void *data;
} Pw_User;

#define RLPW_USER_H
#endif

