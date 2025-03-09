#include <stdio.h>
#include "interface/interface.h"
#include "login/login.h"

int main() {
    UserList* list = create_user_list();
    kLoginState state = load_users_from_file(list, "users.dat");
    if (state != kOK) {
        destroy_user_list(list);
        return state;
    }

    start_interface(list);
    return 0;
}
