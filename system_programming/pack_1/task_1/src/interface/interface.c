#include "interface.h"

void start_interface(UserList *users) {
    Session  *session = create_session();
    kLoginState state;
    TimeState time_state;
    char line[256];

    while (true) {
        if (!session->logged_in) {
            printf("To continue, please, log in or register:\n");
            printf("> l {username} {pin} < --- login\n");
            printf("> r {username} {pin} < --- register\n");

            char command;
            char username[7];
            int pin;

            if (fgets(line, sizeof(line), stdin)) {
                if (sscanf(line, " %c %6s %d", &command, username, &pin) == 3) {
                    if (command == 'l') {
                        state = login(users, session, username, pin);
                        if (state != kOK) {
                            printf("User not found!\n");
                            continue;
                        }
                        printf("You are logged in as %s", session->username);
                    } else if (command == 'r') {
                        state = register_user(users, username, pin);
                        if (state != kOK) {
                            printf("Failed to create user!\n");
                        }
                        state = login(users, session, username, pin);
                        if (state != kOK) {
                            printf("Login Failed!\n");
                            continue;
                        }
                        printf("You are logged in as %s\n", session->username);
                    } else {
                        printf("bad input. Try again!\n");
                    }
                } else {
                    printf("bad input. Try again!\n");
                }
            } else {
                printf("bad input. Try again!\n");
            }

        } else {
            char username[7];
            char date[11];
            int limit;
            char flag[3];
            if (session->remaining_requests < 0) {
                printf("[ %s ] --->", session->username);
            } else if (session->remaining_requests > 0) {
                printf("[ %s { %d } ] --->", session->username, session->remaining_requests);
            } else {
                printf("you ran out of requests limit, logging out!\n");
                state = logout(session);
                if (state != kOK ) {
                    printf("logout error!");
                    continue;
                }
                continue;
            }

            fgets(line, 256, stdin);
            line[strcspn(line, "\n")] = '\0';
            printf("\n");

            if (!strcmp(line, "Logout")){
                state = logout(session);
                if (state != kOK ) {
                    printf("logout error!");
                    continue;
                }
                printf("successfully logged out!\n");
            } else if (!strcmp(line, "Date")) {
                char* date;
                time_state = get_current_date(&date);
                if (time_state != TIME_OK) {
                    printf("Error while getting current date\n");
                    continue;
                }
                printf("%s\n", date);
            } else if (!strcmp(line, "Time")) {
                char* time;
                time_state = get_current_time(&time);
                if (time_state != TIME_OK) {
                    printf("Error while getting current time\n");
                    continue;
                }
                printf("%s\n", date);
            } else if (sscanf(line, "Howmuch %10s %2s", date, flag)) {
                long difference;
                time_state = calculate_time_difference(date, flag, &difference);
                if (time_state != TIME_OK) {
                    printf("Error while getting time difference\n");
                    continue;
                }
                printf("%ld\n", difference);
            }
            else if (sscanf(line, "Sanctions %6s %d", username, &limit) == 2) {
                printf("To confirm your action, write \"12345\"");
                fgets(line, 256, stdin);
                if (strcmp(line, "12345") < 1) {
                    printf("canceling\n");
                    continue;
                }
                kLoginState limit_state = set_request_limit(users, username, limit);
                if (limit_state != kOK) {
                    printf("Error while setting limit\n");
                    continue;
                }
            }
            session->remaining_requests--;
        }
    }
}
