#include "../include/user_vector.h"

User* new_user(const char* name, int pin, int limit) {
    User* user = malloc(sizeof(User));
    if (!user) {
        return NULL;
    }
    user->pin = pin;
    user->limit = limit;
    user->username = strdup(name);
    if (!user->username) {
        free(user);
        return NULL;
    }
    return user;
}

void destroy_user(User* user) {
    free(user->username);
    free(user);
}

UserList* new_user_list(int initial_capacity) {
    UserList* list = malloc(sizeof(UserList));
	list->buffer = malloc(sizeof(User*) * initial_capacity);
	list->capacity = initial_capacity;
	list->len = 0;
	return list;
}

State add_user(User* user, UserList* list) {
    if (list->len + 1 >= list->capacity) {
		User** newBuffer = malloc(list->capacity * sizeof(User*) * 2);
        if (!newBuffer) {
            return STATE_BAD_ALLOCATION;
        }
		for (int i = 0; i < list->len; i++) {
			newBuffer[i] = list->buffer[i];
		}

		free(list->buffer);
		list->buffer = newBuffer;
		list->capacity *= 2;
	}
	list->buffer[list->len] = user;
	list->len++;
    return STATE_OK;
}

void destroy_user_list(UserList* list) {
    for(int i = 0; i < list->len; i++) {
        destroy_user(list->buffer[i]);
    }
    free(list->buffer);
    free(list);
}
