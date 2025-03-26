#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include "user_vector.h"

State load_data(UserList* list, const char* file_path);

State save_data(UserList* list, const char* file_path);

#endif
