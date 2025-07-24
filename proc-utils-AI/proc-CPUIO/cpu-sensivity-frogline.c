#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <dirent.h>

#define CHECK_INTERVAL_MS 2000
#define CPU_SENSIVITY_THRESHOLD 70
#define PROC_NAME "./cpu_sensivity_frogline_output.txt"

static int cpu_over_threshold = 0;
static pthread_t monitor_thread;
static int running = 1;

unsigned long long last_total_user, last_total_user_low, last_total_sys, last_total_idle;

int get_cpu_usage() {
    FILE* file;
    unsigned long long user, nice, system, idle;
    unsigned long long total_user, total_user_low, total_sys, total_idle;
    unsigned long long total, total_diff;
    static int initialized = 0;

    file = fopen("/proc/stat", "r");
    if (!file) return -1;

    fscanf(file, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle);
    fclose(file);

    total_user = user;
    total_user_low = nice;
    total_sys = system;
    total_idle = idle;

    if (!initialized) {
        last_total_user = total_user;
        last_total_user_low = total_user_low;
        last_total_sys = total_sys;
        last_total_idle = total_idle;
        initialized = 1;
        return 0;
    }

    unsigned long long prev_total = last_total_user + last_total_user_low + last_total_sys + last_total_idle;
    unsigned long long curr_total = total_user + total_user_low + total_sys + total_idle;
    total_diff = curr_total - prev_total;

    unsigned long long prev_busy = last_total_user + last_total_user_low + last_total_sys;
    unsigned long long curr_busy = total_user + total_user_low + total_sys;
    unsigned long long busy_diff = curr_busy - prev_busy;

    last_total_user = total_user;
    last_total_user_low = total_user_low;
    last_total_sys = total_sys;
    last_total_idle = total_idle;

    if (total_diff == 0) return 0;

    return (int)((busy_diff * 100) / total_diff);
}

void* monitor_fn(void* arg) {
    while (running) {
        int usage = get_cpu_usage();
        cpu_over_threshold = (usage > CPU_SENSIVITY_THRESHOLD) ? 1 : 0;
        usleep(CHECK_INTERVAL_MS * 1000);
    }
    return NULL;
}

void show_status() {
    FILE* out = fopen(PROC_NAME, "w");
    if (!out) return;

    if (cpu_over_threshold) {
        fprintf(out, "ALERT: CPU threshold exceeded. AI optimization required.\n");
    } else {
        fprintf(out, "NORMAL: CPU usage is within acceptable limits.\n");
    }

    fclose(out);
}

int main() {
    pthread_create(&monitor_thread, NULL, monitor_fn, NULL);
    printf("[frogline] CPU Sensivity Plugin started. Type 'show' or 'exit'\n");

    char cmd[64];
    while (1) {
        printf("Command: ");
        if (!fgets(cmd, sizeof(cmd), stdin)) break;

        if (strncmp(cmd, "show", 4) == 0) {
            show_status();
            system("cat " PROC_NAME);
        } else if (strncmp(cmd, "exit", 4) == 0) {
            break;
        } else {
            printf("Unknown command.\n");
        }
    }

    running = 0;
    pthread_join(monitor_thread, NULL);
    remove(PROC_NAME);
    printf("[frogline] CPU Sensivity Plugin stopped.\n");
    return 0;
}
