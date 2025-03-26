#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    kOK,
    kBAD_ALLOCATION
} kState;

static int filter(const struct dirent *entry) {
    return strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0;
}

static int compare(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

kState list_files(int num, char** directories) {
    for(int i = num == 1 ? 0 : 1; i < num; i++) {
        printf("%s:\n", directories[i]);
        struct dirent** entries;
        int filesNum = scandir(directories[i], &entries, filter, compare);
        for (int j = 0; j < filesNum; j++) {
            printf("%s", entries[j]->d_name, entries[j]->d_type);

            switch (entries[j]->d_type)
            {
            case DT_DIR:
                printf(" directory\n");
                break;
            case DT_REG:
                printf(" file\n");
            case DT_LNK:
                printf(" symlink\n");
            default:
                printf("unknown type\n");
                break;
            }
        }
    }
    return kOK;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        char** directories = malloc(sizeof(char*));
        directories[0] = ".";
        kState result = list_files(1, directories);
        free(directories);
        return result;
    }
    return list_files(argc, argv);
}
