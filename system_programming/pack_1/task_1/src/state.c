#include "../include/state.h"

void log_error(State state) {
    switch(state) {
        case STATE_OK:
            printf("Operation completed successfully\n");
            break;
        case STATE_BAD_ALLOCATION:
            printf("Error: Memory allocation failed\n");
            break;
        case STATE_PARSE_ERROR:
            printf("Error: Failed to parse input\n");
            break;
        case STATE_FILE_ERROR:
            printf("Error: File operation failed\n");
            break;
        case STATE_USER_NOT_FOUND:
            printf("Error: User not found\n");
            break;
        case STATE_USERNAME_TAKEN:
            printf("Error: Username already taken\n");
            break;
        case STATE_INVALID_PIN:
            printf("Error: Invalid PIN (must be positive number)\n");
            break;
        case STATE_INVALID_USERNAME:
            printf("Error: Invalid username format\n");
            break;
        case STATE_INVALID_DATE:
            printf("Error: Invalid date format\n");
            break;
        case STATE_INVALID_FLAG:
            printf("Error: Invalid flag specified\n");
            break;
        case STATE_INVALID_LIMIT:
            printf("Error: Invalid limit value\n");
            break;
        case STATE_DATE_IN_FUTURE:
            printf("Error: Date is in the future\n");
            break;
        default:
            printf("Error: Unknown error code (%d)\n", state);
    }
}
