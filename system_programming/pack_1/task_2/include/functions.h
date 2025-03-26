#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "const.h"

Status copyN(const char* filename, int N);

#endif
