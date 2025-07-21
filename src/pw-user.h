#ifndef RLPW_USER_H

typedef void *(*Pw_Callback)(void *);

typedef struct Pw_User {
    Pw_Callback callback;
    void *data;
} Pw_User;

#define RLPW_USER_H
#endif

