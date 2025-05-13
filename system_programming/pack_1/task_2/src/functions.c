#include "../include/functions.h"

Status xorN(ParseResult* parse_result) {
    if (parse_result == NULL || parse_result->arg == NULL || parse_result->paths == NULL) {
        return STATUS_INVALID_ARG;
    }

    const int N = parse_result->arg->int_arg;
    const char** file_paths = parse_result->paths;
    const int file_count = parse_result->paths_num;

    for (int file_index = 0; file_index < file_count; file_index++) {
        const char* current_file_path = file_paths[file_index];

        FILE* file_handle = fopen(current_file_path, "rb");
        if (file_handle == NULL) {
            return STATUS_CANNOT_OPEN_FILE;
        }

        const size_t bits_requested = 1 << N;
        const size_t full_bytes = bits_requested / 8;
        const size_t remaining_bits = bits_requested % 8;
        const size_t total_bytes = full_bytes + (remaining_bits ? 1 : 0);

        uint8_t* data_block = (uint8_t*)calloc(total_bytes, sizeof(uint8_t));
        uint8_t* xor_result = (uint8_t*)calloc(total_bytes, sizeof(uint8_t));
        if (data_block == NULL || xor_result == NULL) {
            if (data_block) free(data_block);
            if (xor_result) free(xor_result);
            fclose(file_handle);
            return STATUS_BAD_ALLOCATION;
        }

        size_t bytes_read;
        while ((bytes_read = fread(data_block, sizeof(uint8_t), total_bytes, file_handle)) > 0) {
            if (bytes_read < total_bytes) {
                memset(data_block + bytes_read, 0, total_bytes - bytes_read);
            }

            for (size_t i = 0; i < full_bytes; i++) {
                xor_result[i] ^= data_block[i];
            }

            if (remaining_bits > 0 && bytes_read >= full_bytes) {
                const uint8_t mask = (1 << remaining_bits) - 1;
                xor_result[full_bytes] ^= (data_block[full_bytes] & mask);
            }
        }

        printf("XOR result for file '%s' with N=%d (%zu bits):\n",
               current_file_path, N, bits_requested);
        for (size_t i = 0; i < full_bytes; i++) {
            printf("%02x ", xor_result[i]);
        }
        if (remaining_bits > 0) {
            printf("%02x (last %zu bits)", xor_result[full_bytes], remaining_bits);
        }
        printf("\n");

        free(data_block);
        free(xor_result);
        fclose(file_handle);
    }

    return STATUS_OK;
}

Status mask(ParseResult* parse_result) {
    if (parse_result == NULL || parse_result->arg == NULL || parse_result->paths == NULL) {
        return STATUS_INVALID_ARG;
    }

    const uint32_t mask_value = strtoul(parse_result->arg->char_arg, NULL, 16);
    const char** file_paths = parse_result->paths;
    const int file_count = parse_result->paths_num;

    for (int file_index = 0; file_index < file_count; file_index++) {
        const char* current_file_path = file_paths[file_index];

        FILE* file_handle = fopen(current_file_path, "rb");
        if (file_handle == NULL) {
            return STATUS_CANNOT_OPEN_FILE;
        }

        uint32_t current_value;
        size_t match_count = 0;
        while (fread(&current_value, sizeof(uint32_t), 1, file_handle)) {
            if ((current_value & mask_value) == mask_value) {
                match_count++;
            }
        }

        printf("File '%s': %zu values match mask 0x%08x\n",
               current_file_path, match_count, mask_value);

        fclose(file_handle);
    }

    return STATUS_OK;
}

Status copyN(ParseResult* parse_result) {
    if (parse_result == NULL || parse_result->arg == NULL || parse_result->paths == NULL) {
        return STATUS_INVALID_ARG;
    }

    const int copy_count = parse_result->arg->int_arg;
    const char** file_paths = parse_result->paths;
    const int file_count = parse_result->paths_num;

    for (int file_index = 0; file_index < file_count; file_index++) {
        const char* original_file_path = file_paths[file_index];

        for (int copy_index = 1; copy_index <= copy_count; copy_index++) {
            pid_t process_id = fork();
            if (process_id == 0) {
                int name_length = snprintf(NULL, 0, "%s_%d", original_file_path, copy_index);
                char* copy_file_path = malloc(name_length + 1);
                if (copy_file_path == NULL) {
                    exit(STATUS_BAD_ALLOCATION);
                }
                snprintf(copy_file_path, name_length + 1, "%s_%d", original_file_path, copy_index);

                FILE* source_file = fopen(original_file_path, "rb");
                if (source_file == NULL) {
                    free(copy_file_path);
                    exit(STATUS_CANNOT_OPEN_FILE);
                }

                FILE* destination_file = fopen(copy_file_path, "wb");
                if (destination_file == NULL) {
                    fclose(source_file);
                    free(copy_file_path);
                    exit(STATUS_CANNOT_OPEN_FILE);
                }

                uint8_t buffer[4096];
                size_t bytes_read;
                while ((bytes_read = fread(buffer, 1, sizeof(buffer), source_file))) {
                    fwrite(buffer, 1, bytes_read, destination_file);
                }

                fclose(source_file);
                fclose(destination_file);
                free(copy_file_path);
                exit(STATUS_OK);
            }
            else if (process_id < 0) {
                return STATUS_FORK_FAILED;
            }
        }

        for (int i = 0; i < copy_count; i++) {
            wait(NULL);
        }
    }

    return STATUS_OK;
}

Status find(ParseResult* parse_result) {
    if (parse_result == NULL || parse_result->arg == NULL || parse_result->paths == NULL) {
        return STATUS_INVALID_ARG;
    }

    const char* search_string = parse_result->arg->char_arg;
    const char** file_paths = parse_result->paths;
    const int file_count = parse_result->paths_num;
    int any_matches_found = 0;

    for (int file_index = 0; file_index < file_count; file_index++) {
        pid_t process_id = fork();
        if (process_id == 0) {
            const char* current_file_path = file_paths[file_index];

            FILE* file_handle = fopen(current_file_path, "r");
            if (file_handle == NULL) {
                exit(STATUS_CANNOT_OPEN_FILE);
            }

            fseek(file_handle, 0, SEEK_END);
            long file_size = ftell(file_handle);
            fseek(file_handle, 0, SEEK_SET);

            const int search_length = strlen(search_string);
            int match_found = 0;

            for (long position = 0; position <= file_size - search_length; position++) {
                int potential_match = 1;

                for (int offset = 0; offset < search_length; offset++) {
                    fseek(file_handle, position + offset, SEEK_SET);
                    if (fgetc(file_handle) != search_string[offset]) {
                        potential_match = 0;
                        break;
                    }
                }

                if (potential_match) {
                    match_found = 1;
                    break;
                }
            }

            fclose(file_handle);
            exit(match_found ? STATUS_FOUND : STATUS_NOT_FOUND);
        }
        else if (process_id < 0) {
            return STATUS_FORK_FAILED;
        }
    }

    for (int i = 0; i < file_count; i++) {
        int process_status;
        wait(&process_status);

        if (WEXITSTATUS(process_status) == STATUS_FOUND) {
            printf("Found in: %s\n", file_paths[i]);
            any_matches_found = 1;
        }
    }

    return any_matches_found ? STATUS_OK : STATUS_NOT_FOUND;
}
