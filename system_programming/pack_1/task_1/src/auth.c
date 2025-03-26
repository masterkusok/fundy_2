#include "../include/auth.h"
#include <stdio.h>

Session* new_session() {
    Session* session = malloc(sizeof(Session));
    if (!session) {
        return NULL;
    }
    session->limit = 0;
    session->logged_in = false;
    return session;
}

void destroy_session(Session* session) {
    if (session->username) {
        free(session->username);
    }
    free(session);
}

State login(Session* session, UserList* list, const char* username, int pin) {
    for(int i = 0; i < list->len; i++) {
        if(strcmp(list->buffer[i]->username, username) == 0) {
            if (list->buffer[i]->pin == pin) {
                session->username = strdup(username);
                if (!session->username) {
                    return STATE_BAD_ALLOCATION;
                }
                session->logged_in = true;
                session->limit = list->buffer[i]->limit;
                return STATE_OK;
            }
        }
    }
    return STATE_USER_NOT_FOUND;
}

State logout(Session* session) {
    if(session->username) {
        free(session->username);
    }
    session->logged_in = false;
    return STATE_OK;
}

State register_user(UserList* list, const char* username, int pin) {
    if (strlen(username) > 6) {
        return  STATE_INVALID_USERNAME;
    }

    if (pin > 100000 || pin < 0) {
        return STATE_INVALID_PIN;
    }

    for(int i = 0; i < list->len; i++) {
        if(strcmp(list->buffer[i]->username, username) == 0) {
            return STATE_USERNAME_TAKEN;
        }
    }

    User* user = new_user(username, pin, -1);
    if (!user) {
        return STATE_BAD_ALLOCATION;
    }

    State state = add_user(user, list);
    if (state != STATE_OK) {
        destroy_user(user);
        return state;
    }

    return STATE_OK;
}

State set_limit(UserList* list, const char* username, int limit) {
    if (limit < 0) {
        return STATE_INVALID_LIMIT;
    }

    for(int i = 0; i < list->len; i++) {
        if(strcmp(list->buffer[i]->username, username) == 0) {
            list->buffer[i]->limit = limit;
            return STATE_OK;
        }
    }
    return STATE_USER_NOT_FOUND;
}
