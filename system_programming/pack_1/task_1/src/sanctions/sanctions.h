#ifndef SANCTIONS_H
#define SANCTIONS_H

#include "../login/login.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

kLoginState set_request_limit(UserList* user_list, const char* username, int limit);

#endif
