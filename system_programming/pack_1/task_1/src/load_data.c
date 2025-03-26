#include "../include/load_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

State load_data(UserList* list, const char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return STATE_FILE_ERROR;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {

        if (read > 0 && line[read-1] == '\n') {
            line[read-1] = '\0';
        }

        char username[7];
        int pin;
        int limit;

        if (sscanf(line, "%6s %d %d", username, &pin, &limit) != 3) {
            free(line);
            fclose(file);
            return STATE_PARSE_ERROR;
        }

        User* user = new_user(username, pin, limit);
        if (!user) {
            free(line);
            fclose(file);
            return STATE_BAD_ALLOCATION;
        }
        user->limit = limit;

        State state = add_user(user, list);
        if (state != STATE_OK) {
            destroy_user(user);
            free(line);
            fclose(file);
            return state;
        }
    }

    free(line);
    fclose(file);
    return STATE_OK;
}

State save_data(UserList* list, const char* file_path) {
    FILE* file = fopen(file_path, "w");
    if (!file) {
        return STATE_FILE_ERROR;
    }

    for (int i = 0; i < list->len; i++) {
        User* user = list->buffer[i];
        if (fprintf(file, "%s %d %d\n", user->username, user->pin, user->limit) < 0) {
            fclose(file);
            return STATE_FILE_ERROR;
        }
    }

    if (fclose(file) == EOF) {
        return STATE_FILE_ERROR;
    }

    return STATE_OK;
}
