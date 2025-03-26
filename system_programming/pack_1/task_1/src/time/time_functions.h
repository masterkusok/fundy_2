#ifndef TIME_H
#define TIME_H

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

typedef enum {
    TIME_OK,
    TIME_BAD_ALLOCATION,
    TIME_INVALID_DATE,
    TIME_INVALID_FLAG
} TimeState;

TimeState get_current_time(char** time_str);

TimeState get_current_date(char** date_str);

TimeState calculate_time_difference(const char* date, const char* flag, long* difference);

#endif
