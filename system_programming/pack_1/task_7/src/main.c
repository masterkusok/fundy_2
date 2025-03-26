#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>

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

        if (filesNum < 0) {
            perror("scandir failed");
            continue;
        }

        for (int j = 0; j < filesNum; j++) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", directories[i], entries[j]->d_name);

            struct stat file_info;
            if (lstat(full_path, &file_info) != 0) {
                perror("lstat failed");
                continue;
            }

            printf("%-20s %-10s (inode: %lu)\n",
                   entries[j]->d_name,
                   (entries[j]->d_type == DT_DIR) ? "directory" :
                   (entries[j]->d_type == DT_REG) ? "file" :
                   (entries[j]->d_type == DT_LNK) ? "symlink" : "unknown",
                   file_info.st_ino);

            free(entries[j]);
        }

        free(entries);
    }
    return kOK;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        char** directories = malloc(sizeof(char*));
        if (!directories) {
            perror("malloc failed");
            return kBAD_ALLOCATION;
        }
        directories[0] = ".";
        kState result = list_files(1, directories);
        free(directories);
        return result;
    }
    return list_files(argc, argv);
}
