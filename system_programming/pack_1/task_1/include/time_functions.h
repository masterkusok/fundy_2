#ifndef TIME_H
#define TIME_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "state.h"


State get_current_time(char** time_str);

State get_current_date(char** date_str);

State calculate_time_difference(const char* date, const char* flag, long* difference);

#endif
