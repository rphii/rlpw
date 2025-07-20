#ifndef BGW_USER_H

typedef void *(*Bgw_Callback)(void *);

typedef struct Bgw_User {
    Bgw_Callback callback;
    void *data;
} Bgw_User;

#define BGW_USER_H
#endif

