#include "login.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INITIAL_CAPACITY 10
#define MAX_USERNAME_LENGTH 6

kLoginState get_request_limit(const char* username, int* limit) {
    FILE* file = fopen(SANCTIONS_FILE, "r");
    if (!file) {
        return kINTERNAL_ERROR;
    }

    char file_username[7];
    int lim;
    while (fscanf(file, "%6s %d", file_username, &lim) == 2) {
        if (strcmp(file_username, username) == 0) {
            fclose(file);
            *limit = lim;
            return kOK;
        }
    }

    fclose(file);
    *limit = -1;
    return kOK;
}

kLoginState login(UserList* user_list, Session* session, const char* username, int pin) {
    for (int i = 0; i < user_list->user_count; i++) {
        if (strcmp(user_list->users[i].username, username) == 0) {
            if (user_list->users[i].pin == pin) {
                session->username = strdup(username);
                if (!session->username) {
                    return kBAD_ALLOCATION;
                }
                int limit;
                if (get_request_limit(username, &limit) != kOK) {
                    free(session->username);
                    return kINTERNAL_ERROR;
                }
                session->logged_in = true;
                session->remaining_requests = limit;
                return kOK;
            } else {
                return kWRONG_PIN;
            }
        }
    }
    return kUNKNOWN_USER;
}

kLoginState register_user(UserList* user_list, const char* username, int pin) {
    if (strlen(username) > MAX_USERNAME_LENGTH) {
        return kINTERNAL_ERROR;
    }

    for (int i = 0; i < user_list->user_count; i++) {
        if (strcmp(user_list->users[i].username, username) == 0) {
            return kUSERNAME_ALREADY_TAKEN;
        }
    }

    if (pin > 100000 || pin < 0) {
        return kINVALID_PIN;
    }

    if (user_list->user_count >= user_list->capacity) {
        int new_capacity = user_list->capacity * 2;
        User* new_users = (User*)realloc(user_list->users, new_capacity * sizeof(User));
        if (!new_users) {
            return kBAD_ALLOCATION;
        }
        user_list->users = new_users;
        user_list->capacity = new_capacity;
    }

    user_list->users[user_list->user_count].username = strdup(username);
    if (!user_list->users[user_list->user_count].username) {
        return kBAD_ALLOCATION;
    }
    user_list->users[user_list->user_count].pin = pin;
    user_list->user_count++;
    save_users_to_file(user_list, "users.dat");
    return kOK;
}

kLoginState logout(Session* session) {
    if (session->username) {
        free(session->username);
        session->username = NULL;
        session->remaining_requests = 0;
        session->logged_in = false;
    }
    return kOK;
}

void save_users_to_file(const UserList* user_list, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    fwrite(&user_list->user_count, sizeof(int), 1, file);
    for (int i = 0; i < user_list->user_count; i++) {
        int username_length = strlen(user_list->users[i].username) + 1;
        fwrite(&username_length, sizeof(int), 1, file);
        fwrite(user_list->users[i].username, sizeof(char), username_length, file);
        fwrite(&user_list->users[i].pin, sizeof(int), 1, file);
    }

    fclose(file);
}

kLoginState load_users_from_file(UserList* user_list, const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file for reading");
        return kINTERNAL_ERROR;
    }

    fread(&user_list->user_count, sizeof(int), 1, file);
    user_list->capacity = user_list->user_count;
    user_list->users = (User*)malloc(user_list->capacity * sizeof(User));
    if (!user_list->users) {
        fclose(file);
        return kBAD_ALLOCATION;
    }

    for (int i = 0; i < user_list->user_count; i++) {
        int username_length;
        fread(&username_length, sizeof(int), 1, file);
        user_list->users[i].username = (char*)malloc(username_length * sizeof(char));
        if (!user_list->users[i].username) {
            for (int j = 0; j < i; j++) {
                free(user_list->users[j].username);
            }
            free(user_list->users);
            fclose(file);
            return kBAD_ALLOCATION;
        }
        fread(user_list->users[i].username, sizeof(char), username_length, file);
        fread(&user_list->users[i].pin, sizeof(int), 1, file);
    }

    fclose(file);
    return kOK;
}

Session* create_session() {
    Session* session = (Session*)malloc(sizeof(Session));
    if (session) {
        session->username = NULL;
        session->remaining_requests = -1;
        session->logged_in = false;
    }
    return session;
}

void destroy_session(Session* session) {
    if (session) {
        if (session->username) {
            free(session->username);
        }
        free(session);
    }
}

UserList* create_user_list() {
    UserList* user_list = (UserList*)malloc(sizeof(UserList));
    if (user_list) {
        user_list->users = (User*)malloc(INITIAL_CAPACITY * sizeof(User));
        if (user_list->users) {
            user_list->user_count = 0;
            user_list->capacity = INITIAL_CAPACITY;
        } else {
            free(user_list);
            user_list = NULL;
        }
    }
    return user_list;
}

void destroy_user_list(UserList* user_list) {
    if (user_list) {
        for (int i = 0; i < user_list->user_count; i++) {
            free(user_list->users[i].username);
        }
        free(user_list->users);
        free(user_list);
    }
}
