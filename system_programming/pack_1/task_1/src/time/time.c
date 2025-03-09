#include "time_functions.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

TimeState get_current_time(char** time_str) {
    time_t raw_time;
    struct tm* time_info;
    *time_str = (char*)malloc(9 * sizeof(char));

    if (!*time_str) {
        return TIME_BAD_ALLOCATION;
    }

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(*time_str, 9, "%H:%M:%S", time_info);
    return TIME_OK;
}

TimeState get_current_date(char** date_str) {
    time_t raw_time;
    struct tm* time_info;
    *date_str = (char*)malloc(11 * sizeof(char));

    if (!*date_str) {
        return TIME_BAD_ALLOCATION;
    }

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(*date_str, 11, "%d:%m:%Y", time_info);
    return TIME_OK;
}

TimeState calculate_time_difference(const char* date, const char* flag, long* difference) {
    struct tm tm_date = {0};
    time_t raw_time;
    double seconds;

    if (strptime(date, "%d:%m:%Y", &tm_date) == NULL) {
        return TIME_INVALID_DATE;
    }

    time(&raw_time);

    time_t date_time = mktime(&tm_date);

    seconds = difftime(raw_time, date_time);

    if (strcmp(flag, "-s") == 0) {
        *difference = (long)seconds;
    } else if (strcmp(flag, "-m") == 0) {
        *difference = (long)(seconds / 60);
    } else if (strcmp(flag, "-h") == 0) {
        *difference = (long)(seconds / 3600);
    } else if (strcmp(flag, "-y") == 0) {
        *difference = (long)(seconds / (3600 * 24 * 365));
    } else {
        return TIME_INVALID_FLAG;
    }

    return TIME_OK;
}
