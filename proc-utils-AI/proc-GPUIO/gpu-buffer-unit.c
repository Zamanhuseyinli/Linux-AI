#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // This header defines CLOCK_REALTIME
#include <pthread.h>
#include <unistd.h>

#define DEFAULT_ENTRY_LEN 128
#define MAX_ENTRIES 512
#define CONFIG_PATH "/var/AI-stump/GPU-runtime.yml"

int buffer_max = 64;
int write_index = 0;
pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

struct gpu_entry {
    struct timespec timestamp;
    char data[DEFAULT_ENTRY_LEN];
};

struct gpu_entry *buffer_entries;

int load_yaml_config() {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp)
        return -1;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "buffer_size:")) {
            int val;
            if (sscanf(line, "buffer_size: %d", &val) == 1) {
                if (val >= 1 && val <= MAX_ENTRIES)
                    buffer_max = val;
            }
        }
    }

    fclose(fp);
    return 0;
}

void print_buffer() {
    pthread_mutex_lock(&buffer_lock);
    for (int i = 0; i < write_index; ++i) {
        long ms = buffer_entries[i].timestamp.tv_sec * 1000 + buffer_entries[i].timestamp.tv_nsec / 1000000;
        printf("[%d] (%ld ms) %s\n", i, ms, buffer_entries[i].data);
    }
    pthread_mutex_unlock(&buffer_lock);
}

void write_entry(const char *input) {
    pthread_mutex_lock(&buffer_lock);

    if (write_index >= buffer_max) {
        memmove(&buffer_entries[0], &buffer_entries[1], sizeof(struct gpu_entry) * (buffer_max - 1));
        write_index = buffer_max - 1;
    }

    clock_gettime(CLOCK_REALTIME, &buffer_entries[write_index].timestamp);
    strncpy(buffer_entries[write_index].data, input, DEFAULT_ENTRY_LEN - 1);
    buffer_entries[write_index].data[DEFAULT_ENTRY_LEN - 1] = '\0';
    write_index++;

    pthread_mutex_unlock(&buffer_lock);
}

void change_buffer_size(int new_size) {
    if (new_size >= 1 && new_size <= MAX_ENTRIES) {
        pthread_mutex_lock(&buffer_lock);
        buffer_max = new_size;
        write_index = 0;
        memset(buffer_entries, 0, sizeof(struct gpu_entry) * MAX_ENTRIES);
        pthread_mutex_unlock(&buffer_lock);
        printf("Buffer size changed to %d\n", buffer_max);
    } else {
        printf("Invalid buffer size.\n");
    }
}

int main() {
    buffer_entries = calloc(MAX_ENTRIES, sizeof(struct gpu_entry));
    if (!buffer_entries) {
        perror("Failed to allocate buffer");
        return 1;
    }

    load_yaml_config();
    printf("GPU Buffer Userspace Logger Started [buffer_max=%d]\n", buffer_max);

    char command[256];
    while (1) {
        printf("\nCommand (write/print/set/exit): ");
        if (!fgets(command, sizeof(command), stdin)) break;

        if (strncmp(command, "write ", 6) == 0) {
            write_entry(command + 6);
        } else if (strncmp(command, "print", 5) == 0) {
            print_buffer();
        } else if (strncmp(command, "set ", 4) == 0) {
            int new_size = atoi(command + 4);
            change_buffer_size(new_size);
        } else if (strncmp(command, "exit", 4) == 0) {
            break;
        } else {
            printf("Unknown command.\n");
        }
    }

    free(buffer_entries);
    return 0;
}
