#include <stdio.h>

#include "../include/const.h"
#include "../include/arg_parse.h"
#include "../include/functions.h"

int main(int argc, char **argv) {
    ParseResult* result = parse_arguments(argc, argv);
    if(result->status != STATUS_OK) {
        printf("status: %d\n", result->status);
        free(result);
        return result->status;
    }

    Status (*handlers[4])(ParseResult*) = {
        xorN,
        mask,
        copyN,
        find
    };
    Status status = handlers[result->flag](result);
    printf("%d", status);
    return status;
}
