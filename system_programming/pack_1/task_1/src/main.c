#include "../include/state.h"
#include "../include/user_vector.h"
#include "../include/load_data.h"
#include "../include/auth.h"
#include "../include/time_functions.h"
#include "../include/state.h"

State run_interface(Session* session, UserList* list) {
    State state;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while(true) {
        if (line) {
            free(line);
            line = NULL;
            len = 0;
        }

        if(!session->logged_in) {
            printf("\nYou are not logged in!\n");
            printf(">l {username} {pin} < ------------ login\n");
            printf(">r {username} {pin} < ------------ register\n");
            printf(">exit <--------------------------- exit\n");
            printf("> ");

            read = getline(&line, &len, stdin);
            if (read == -1) {
                continue;
            }

            if (read > 0 && line[read-1] == '\n') {
                line[read-1] = '\0';
            }

            if (strcmp(line, "exit") == 0) {
                break;
            }

            char command;
            char username[7];
            int pin;

            if(sscanf(line, "%c %6s %d", &command, username, &pin) != 3) {
                printf("Invalid command format\n");
                continue;
            }

            if (command == 'l') {
                state = login(session, list, username, pin);
                if (state == STATE_OK) {
                    printf("Successfully logged in as %s\n", username);
                } else {
                    printf("Login failed\n");
                    log_error(state);
                }
                continue;
            }

            if (command == 'r') {
                state = register_user(list, username, pin);
                if (state == STATE_OK) {
                    printf("Registered successfully! Now login!\n");
                } else {
                    printf("Registration failed\n");
                    log_error(state);
                }
                continue;
            }
        } else {
            printf("\n");
            if (session->limit == -1) {
                printf("[ @%s ] > ", session->username);
            } else if (session->limit > 0) {
                printf("[ @%s -> %d ] > ", session->username, session->limit);
            } else {
                logout(session);
                continue;
            }

            read = getline(&line, &len, stdin);
            if (read == -1) {
                continue;
            }

            if (read > 0 && line[read-1] == '\n') {
                line[read-1] = '\0';
            }

            if (strcmp(line, "exit") == 0) {
                break;
            }

            if (strcmp(line, "logout") == 0) {
                logout(session);
                printf("Logged out successfully\n------------------------\n\n");
                continue;
            }

            if (strcmp(line, "Time") == 0) {
                char* time;
                state = get_current_time(&time);
                if (state != STATE_OK) {
                    log_error(state);
                    continue;
                }
                printf("Current time: %s\n", time);
                if (session->limit > 0) {
                    session->limit--;
                }
                free(time);
                continue;
            }

            if (strcmp(line, "Date") == 0) {
                char* date;
                state = get_current_date(&date);
                if (state != STATE_OK) {
                    log_error(state);
                    continue;
                }
                printf("Current date: %s\n", date);
                if (session->limit > 0) {
                    session->limit--;
                }
                free(date);
                continue;
            }

            char date[11];
            int limit;
            char flag[3];
            if (sscanf(line, "Howmuch %10s %2s", date, flag)) {
                long difference;
                state = calculate_time_difference(date, flag, &difference);
                if (state != STATE_OK) {
                    log_error(state);
                    continue;
                }
                printf("%ld\n", difference);
                if(session->limit > 0) {
                    session->limit--;
                }
                continue;
            }

            char username[7];

            if (sscanf(line, "Sanctions %6s %d", username, &limit) == 2) {
                if (strcmp(username, session->username) == 0) {
                    printf("You cannot limit yourself!\n");
                    continue;
                }

                if (session->limit > 0) {
                    session->limit--;
                } else if (session->limit == 0) {
                    printf("No more actions available!\n");
                    continue;
                }

                printf("To confirm your action, write 12345\n");

                read = getline(&line, &len, stdin);
                if (read == -1) {
                    continue;
                }

                if (read > 0 && line[read-1] == '\n') {
                    line[read-1] = '\0';
                }

                if (strcmp(line, "12345") != 0) {
                    printf("Canceling\n");
                    continue;
                }

                state = set_limit(list, username, limit);
                if (state != STATE_OK) {
                    printf("Error while setting limit\n");
                    log_error(state);
                    continue;
                }
                printf("Limit set successfully!\n");
                continue;
            }

            printf("Unknown command\n");
        }
    }

    if (line) {
        free(line);
    }

    return STATE_OK;
}

int main(int argc, char** argv) {
    UserList* list = new_user_list(1);
    if (!list) {
        fprintf(stderr, "Failed to create user list\n");
        return STATE_BAD_ALLOCATION;
    }

    State state = load_data(list, "users.dat");
    if (state != STATE_OK) {
        log_error(state);
    }

    Session* session = new_session();
    if (!session) {
        destroy_user_list(list);
        fprintf(stderr, "Failed to create session\n");
        return STATE_BAD_ALLOCATION;
    }

    state = run_interface(session, list);

    state = save_data(list, "users.dat");
    if (state != STATE_OK) {
        fprintf(stderr, "Warning: Failed to save user data (%d)\n", state);
    }

    destroy_session(session);
    destroy_user_list(list);

    return 0;
}
