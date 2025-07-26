#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <stddef.h>

#define DEFAULT_ENTRY_LEN 128
#define MAX_ENTRIES 512
#define CONFIG_PATH "/var/AI-stump/AI-runtime.yml"

struct buffer_entry {
    struct timespec timestamp;
    char data[DEFAULT_ENTRY_LEN];
};

static struct buffer_entry *buffer_entries;
static int write_index = 0;
static int buffer_max = 64;
static pthread_mutex_t buffer_lock = PTHREAD_MUTEX_INITIALIZER;

int load_yaml_config() {
    FILE *fp = fopen(CONFIG_PATH, "r");
    if (!fp) return -1;

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

void write_entry(const char *input) {
    pthread_mutex_lock(&buffer_lock);

    if (write_index >= buffer_max) {
        memmove(&buffer_entries[0], &buffer_entries[1], sizeof(struct buffer_entry) * (buffer_max - 1));
        write_index = buffer_max - 1;
    }

    clock_gettime(CLOCK_MONOTONIC, &buffer_entries[write_index].timestamp);
    strncpy(buffer_entries[write_index].data, input, DEFAULT_ENTRY_LEN - 1);
    buffer_entries[write_index].data[DEFAULT_ENTRY_LEN - 1] = '\0';
    write_index++;

    pthread_mutex_unlock(&buffer_lock);
}

void print_buffer() {
    pthread_mutex_lock(&buffer_lock);
    for (int i = 0; i < write_index; ++i) {
        long ms = buffer_entries[i].timestamp.tv_sec * 1000 +
                  buffer_entries[i].timestamp.tv_nsec / 1000000;
        printf("[%d] (%ld ms) %s\n", i, ms, buffer_entries[i].data);
    }
    pthread_mutex_unlock(&buffer_lock);
}

void set_buffer_size(int new_size) {
    if (new_size < 1 || new_size > MAX_ENTRIES) {
        printf("Invalid size.\n");
        return;
    }

    pthread_mutex_lock(&buffer_lock);
    buffer_max = new_size;
    write_index = 0;
    memset(buffer_entries, 0, sizeof(struct buffer_entry) * MAX_ENTRIES);
    pthread_mutex_unlock(&buffer_lock);
    printf("Buffer size changed to %d\n", buffer_max);
}

int init_cpu_buffer_unit(void) {
    buffer_entries = calloc(MAX_ENTRIES, sizeof(struct buffer_entry));
    if (!buffer_entries) {
        perror("Allocation failed");
        return 1;
    }

    load_yaml_config();
    printf("[cpu-buffer-unit] Initialized. buffer_max: %d\n", buffer_max);

    char command[256];
    while (1) {
        printf("\nCommand (write <data> / print / set <number> / exit): ");
        if (!fgets(command, sizeof(command), stdin)) break;

        if (strncmp(command, "write ", 6) == 0) {
            write_entry(command + 6);
        } else if (strncmp(command, "print", 5) == 0) {
            print_buffer();
        } else if (strncmp(command, "set ", 4) == 0) {
            int val = atoi(command + 4);
            set_buffer_size(val);
        } else if (strncmp(command, "exit", 4) == 0) {
            break;
        } else {
            printf("Unknown command.\n");
        }
    }

    free(buffer_entries);
    return 0;
}
