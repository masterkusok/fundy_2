#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "arg_parse.h"
#include "const.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "const.h"
#include <sys/wait.h>
#include <string.h>s

Status copyN(ParseResult*);
Status xorN(ParseResult*);
Status mask(ParseResult*);
Status find(ParseResult*);

#endif
