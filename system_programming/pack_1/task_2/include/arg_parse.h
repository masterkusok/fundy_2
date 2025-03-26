#ifndef ARG_PARSE_H
#define ARG_PARSE_H

#include "const.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef enum {
    FLAG_XOR,
    FLAG_MASK,
    FLAG_COPY,
    FLAG_FIND,
    FLAG_UNKNOWN
} Flag;

typedef struct {
    char* char_arg;
    int int_arg;
} Argument;

typedef struct {
    Status status;
    Flag flag;
    Argument* arg;
    char** paths;
    int paths_num;
} ParseResult;

ParseResult* parse_arguments(int argc, char** argv);

void clear_result(ParseResult* result);

#endif
