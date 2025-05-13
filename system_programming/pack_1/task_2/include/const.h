#ifndef CONST_H
#define CONST_H

#define BLOCK_SIZE = 1024

#define CHAR_FLAG_XOR "--xor"
#define CHAR_FLAG_MASK "--mask"
#define CHAR_FLAG_COPY "--copy"
#define CHAR_FLAG_FIND "--find"

typedef enum {
    STATUS_OK,
    STATUS_BAD_ALLOCATION,
    STATUS_INVALID_ARG,
    STATUS_INVALID_ARG_NUM,
    STATUS_PARSE_ERROR,
    STATUS_CANNOT_OPEN_FILE,
    STATUS_FORK_FAILED,
    STATUS_FOUND,
    STATUS_NOT_FOUND,
} Status;

#endif
