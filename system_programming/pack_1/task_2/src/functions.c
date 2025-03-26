#include "../include/functions.h"
#include <sys/wait.h>

Status copyN(const char* filename, int N) {
    // Валидация входных параметров
    if (!filename || N <= 0) {
        return STATUS_INVALID_ARG;
    }

    key_t key = ftok("copyN", 65);
    if (key == -1) {
        return STATUS_PARSE_ERROR;
    }

    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) {
        return STATUS_PARSE_ERROR;
    }

    int* counter = (int*)shmat(shmid, NULL, 0);
    if (counter == (int*)-1) {
        shmctl(shmid, IPC_RMID, NULL);
        return STATUS_PARSE_ERROR;
    }

    *counter = 0;
    Status overall_status = STATUS_OK;

    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            overall_status = STATUS_PARSE_ERROR;
            continue; // Продолжаем попытки с остальными копиями
        }

        if (pid == 0) { // Дочерний процесс
            char new_filename[256];
            snprintf(new_filename, sizeof(new_filename), "%s_%d", filename, i + 1);

            int src_fd = open(filename, O_RDONLY);
            if (src_fd == -1) {
                _exit(STATUS_CANNOT_OPEN_FILE);
            }

            int dest_fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (dest_fd == -1) {
                close(src_fd);
                _exit(STATUS_CANNOT_OPEN_FILE);
            }

            char buffer[1024];
            ssize_t bytes_read, bytes_written;
            while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
                bytes_written = write(dest_fd, buffer, bytes_read);
                if (bytes_written != bytes_read) {
                    close(src_fd);
                    close(dest_fd);
                    _exit(STATUS_PARSE_ERROR);
                }
            }

            close(src_fd);
            close(dest_fd);

            // Увеличиваем счетчик в разделяемой памяти
            (*counter)++;
            shmdt(counter);
            _exit(STATUS_OK);
        }
    }

    // Ожидаем завершения всех дочерних процессов
    int child_status;
    for (int i = 0; i < N; i++) {
        wait(&child_status);
        if (WIFEXITED(child_status)) {
            int exit_status = WEXITSTATUS(child_status);
            if (exit_status != STATUS_OK) {
                overall_status = exit_status;
            }
        } else {
            overall_status = STATUS_PARSE_ERROR;
        }
    }

    // Проверяем количество успешных копий
    if (*counter != N) {
        overall_status = STATUS_PARSE_ERROR;
    }

    // Освобождаем ресурсы
    shmdt(counter);
    shmctl(shmid, IPC_RMID, NULL);

    return overall_status;
}
