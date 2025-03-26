#include "../include/time_functions.h"

State get_current_time(char** time_str) {
    time_t raw_time;
    struct tm* time_info;
    *time_str = (char*)malloc(9 * sizeof(char));

    if (!*time_str) {
        return STATE_BAD_ALLOCATION;
    }

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(*time_str, 9, "%H:%M:%S", time_info);
    return STATE_OK;
}

State get_current_date(char** date_str) {
    time_t raw_time;
    struct tm* time_info;
    *date_str = (char*)malloc(11 * sizeof(char));

    if (!*date_str) {
        return STATE_BAD_ALLOCATION;
    }

    time(&raw_time);
    time_info = localtime(&raw_time);

    strftime(*date_str, 11, "%d:%m:%Y", time_info);
    return STATE_OK;
}

bool is_leap_year(int year) {
    if (year % 4 != 0) {
        return false;
    } else if (year % 100 != 0) {
        return true;
    } else if (year % 400 != 0) {
        return false;
    } else {
        return true;
    }
}

bool is_valid_date(int day, int month, int year) {
    if (year < 1900 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && is_leap_year(year)) {
        days_in_month[1] = 29;
    }

    return day <= days_in_month[month - 1];
}

State calculate_time_difference(const char* date, const char* flag, long* difference) {
    struct tm tm_date = {0};
    time_t raw_time;
    double seconds;
    int day, month, year;

    if (sscanf(date, "%d:%d:%d", &day, &month, &year) != 3) {
        return STATE_INVALID_DATE;
    }

    if (!is_valid_date(day, month, year)) {
        return STATE_INVALID_DATE;
    }

    tm_date.tm_mday = day;
    tm_date.tm_mon = month - 1;
    tm_date.tm_year = year - 1900;
    tm_date.tm_isdst = -1;

    time(&raw_time);

    time_t date_time = mktime(&tm_date);
    if (date_time == -1) {
        return STATE_INVALID_DATE;
    }

    seconds = difftime(raw_time, date_time);
    if (seconds < 0) {
        return STATE_DATE_IN_FUTURE;
    }

    if (strcmp(flag, "-s") == 0) {
        *difference = (long)seconds;
    } else if (strcmp(flag, "-m") == 0) {
        *difference = (long)(seconds / 60);
    } else if (strcmp(flag, "-h") == 0) {
        *difference = (long)(seconds / 3600);
    } else if (strcmp(flag, "-d") == 0) {
        *difference = (long)(seconds / (3600 * 24));
    } else if (strcmp(flag, "-y") == 0) {
        struct tm tm_now;
        localtime_r(&raw_time, &tm_now);

        int year_diff = tm_now.tm_year - tm_date.tm_year;
        if (tm_now.tm_mon < tm_date.tm_mon ||
            (tm_now.tm_mon == tm_date.tm_mon && tm_now.tm_mday < tm_date.tm_mday)) {
            year_diff--;
        }
        *difference = year_diff;
    } else {
        return STATE_INVALID_FLAG;
    }

    return STATE_OK;
}
