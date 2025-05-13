#include "../include/arg_parse.h"

ParseResult* new_parse_result() {
    ParseResult* result = malloc(sizeof(ParseResult));
    if (!result) {
        return NULL;
    }
    result->flag = FLAG_UNKNOWN;
    result->status = STATUS_OK;
    result->paths = NULL;
    result->paths_num = 0;
    result->arg = NULL;
    return result;
}

Argument* new_argument(int n, char* s) {
    Argument* arg = malloc(sizeof(Argument));
    if (!arg) {
        return NULL;
    }

    arg->int_arg = n;
    arg->char_arg = s;

    return arg;
}

Flag flag_from_string(const char* s) {
    if (!strncmp(s, CHAR_FLAG_XOR, strlen(CHAR_FLAG_XOR))) {
        return FLAG_XOR;
    }
    if (!strncmp(s, CHAR_FLAG_COPY, strlen(CHAR_FLAG_COPY))) {
        return FLAG_COPY;
    }
    if (!strcmp(s, CHAR_FLAG_FIND)) {
        return FLAG_FIND;
    }
    if (!strcmp(s, CHAR_FLAG_MASK)) {
        return FLAG_MASK;
    }
    return FLAG_UNKNOWN;
}

ParseResult* parse_arguments(int argc, char** argv) {
    ParseResult* result = new_parse_result();
    if (!result)  {
        return NULL;
    }

    if (argc < 3) {
        result->status = STATUS_INVALID_ARG_NUM;
        return result;
    }

    int paths_num = argc - 3;
    result->flag = flag_from_string(argv[argc-2]);
    if (result->flag == FLAG_UNKNOWN) {
        result->flag = flag_from_string(argv[argc-1]);
        if (result->flag == FLAG_UNKNOWN) {
            result->status = STATUS_INVALID_ARG;
            return result;
        }
        paths_num++;
    }

    char** result_paths = malloc(sizeof(char*) * paths_num);
    if (!result_paths) {
        result->status = STATUS_BAD_ALLOCATION;
        return result;
    }

    for (int i = 1; i < 1 + paths_num; i++) {
        char * copy = malloc((strlen(argv[i]) + 1) * sizeof(char));
        if (!copy) {
            for (int j = i - 1; j >= 0; j--) {
                free(result_paths[j]);
            }
            free(result_paths);
            result->status = STATUS_BAD_ALLOCATION;
            return result;
        }

        strcpy(copy, argv[i]);
        result_paths[i-1] = copy;
    }

    result->paths = result_paths;
    result->paths_num = paths_num;

    if (result->flag == FLAG_XOR) {

        char* flag = argv[argc-1];
        if (strlen(flag) != strlen(CHAR_FLAG_XOR) + 1 || !isdigit(flag[strlen(CHAR_FLAG_XOR)])) {
            result->status = STATUS_INVALID_ARG;
            return result;
        }
        int n = flag[strlen(CHAR_FLAG_XOR)] - '0';
        Argument* arg = new_argument(n, NULL);
        if (!arg) {
            result->status = STATUS_BAD_ALLOCATION;
            return result;
        }
        result->arg = arg;
        return result;
    }
    if (result->flag == FLAG_COPY) {
        const char* flag = argv[argc-1];
        const char* prefix = "--copy";

        if (strncmp(flag, prefix, strlen(prefix)) != 0) {
            result->status = STATUS_INVALID_ARG;
            return result;
        }

        int n = atoi(flag + strlen(prefix));
        if (n <= 0) {
            result->status = STATUS_INVALID_ARG;
            return result;
        }
        Argument* arg = new_argument(n, NULL);
        if (!arg) {
            result->status = STATUS_BAD_ALLOCATION;
            return result;
        }
        result->arg = arg;
        return result;
    }

    if (result->flag == FLAG_FIND || result->flag == FLAG_MASK) {
        char * copy = malloc((strlen(argv[argc-1]) + 1) * sizeof(char));
        if (!copy) {
            result->status = STATUS_BAD_ALLOCATION;
            return result;
        }
        strcpy(copy, argv[argc-1]);
        Argument* arg = new_argument(0, copy);
        if (!arg) {
            free(copy);
            result->status = STATUS_BAD_ALLOCATION;
            return result;
        }
        result->arg = arg;
        return result;
    }

    return result;
}
