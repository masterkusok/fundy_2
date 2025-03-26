#ifndef AUTH_H
#define AUTH_H

#include <stdbool.h>

#include "user_vector.h"

typedef struct {
    int limit;
    char* username;
    bool logged_in;
} Session;

Session* new_session();

void destroy_session(Session* session);

State login(Session* session, UserList* list, const char* username, int pin);

State logout(Session* session);

State register_user(UserList* list, const char* username, int pin);

State set_limit(UserList* list, const char* username, int limit);

#endif
