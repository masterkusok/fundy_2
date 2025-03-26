#ifndef LOGIN_H
#define LOGIN_H
#include <stdbool.h>

typedef enum {
    kOK,
    kUNKNOWN_USER,
    kWRONG_PIN,
    kUSERNAME_ALREADY_TAKEN,
    kINTERNAL_ERROR,
    kBAD_ALLOCATION,
    kINVALID_PIN,
} kLoginState;

#define SANCTIONS_FILE "sanctions.dat"

typedef struct {
    char* username;
    int remaining_requests;
    bool logged_in;
} Session;

typedef struct {
    char* username;
    int pin;
} User;

typedef struct {
    User* users;
    int user_count;
    int capacity;
} UserList;

kLoginState login(UserList* user_list, Session* session, const char* username, int pin);
kLoginState register_user(UserList* user_list, const char* username, int pin);
kLoginState logout(Session* session);
void save_users_to_file(const UserList* user_list, const char* filename);
kLoginState load_users_from_file(UserList* user_list, const char* filename);
Session* create_session();
void destroy_session(Session* session);
UserList* create_user_list();
void destroy_user_list(UserList* user_list);

#endif
