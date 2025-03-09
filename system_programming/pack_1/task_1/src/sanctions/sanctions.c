#include "sanctions.h"

#define SANCTIONS_FILE "sanctions.dat"

kLoginState set_request_limit(UserList* user_list, const char* username, int limit) {
    bool user_exists = false;
    for (int i = 0; i < user_list->user_count; i++) {
        if (strcmp(user_list->users[i].username, username) == 0) {
            user_exists = true;
            break;
        }
    }

    if (!user_exists) {
        return kUNKNOWN_USER;
    }

    FILE* file = fopen(SANCTIONS_FILE, "a");
    if (!file) {
        return kINTERNAL_ERROR;
    }

    fprintf(file, "%s %d\n", username, limit);
    fclose(file);

    return kOK;
}
