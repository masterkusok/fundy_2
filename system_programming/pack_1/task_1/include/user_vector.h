#ifndef USER_VECTOR_H
#define USER_VECTOR_H

#include <stdlib.h>
#include <string.h>

#include "state.h"

typedef struct {
    char* username;
    int pin;
    int limit;
} User;

typedef struct {
    User** buffer;
    int len;
    int capacity;
} UserList;

User* new_user(const char* name, int pin, int limit);

void destroy_user(User* user);

UserList* new_user_list(int initial_capacity);

State add_user(User* user, UserList* list);

void destroy_user_list(UserList* user);

#endif
