#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include "interface/interface.h"
#include "login/login.h"
#include "sanctions/sanctions.h"

#define USERS_FILE "users.dat"

int ensure_files_exist() {
    if(access(USERS_FILE, F_OK)) {
        FILE* users = fopen(USERS_FILE, "w");
        if (!users) {
            return 0;
        }
        fclose(users);
    }

    if(access(SANCTIONS_FILE, F_OK)) {
        FILE* sanctions = fopen(SANCTIONS_FILE, "w");
        if (!sanctions) {
            return 0;
        }
        fclose(sanctions);
    }
    return 1;
}

int main() {
    if (!ensure_files_exist()){
        printf("failed to create program files\n");
        return 1;
    }
    UserList* list = create_user_list();
    kLoginState state = load_users_from_file(list, USERS_FILE);
    if (state != kOK) {
        destroy_user_list(list);
        return state;
    }

    start_interface(list);
    destroy_user_list(list);
    return 0;
}
