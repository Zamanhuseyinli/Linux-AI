#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_CPUS 128
#define BUF_SIZE 128

static volatile int runner = 1;
static volatile int turbo_enabled = 0;
static char current_policy[BUF_SIZE] = "ondemand";

void safe_write(const char *path, const char *data) {
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[cpu_manager] ERROR opening %s: %s\n", path, strerror(errno));
        return;
    }
    if (write(fd, data, strlen(data)) < 0) {
        fprintf(stderr, "[cpu_manager] ERROR writing %s: %s\n", path, strerror(errno));
    }
    close(fd);
}

void set_governor_all(const char *governor) {
    char path[BUF_SIZE];
    for (int cpu = 0; cpu < MAX_CPUS; cpu++) {
        snprintf(path, sizeof(path),
                 "/sys/devices/system/cpu/cpu%d/cpufreq/scaling_governor", cpu);
        if (access(path, W_OK) == 0) {
            safe_write(path, governor);
        }
    }
    printf("[cpu_manager] Governor set to %s on all CPUs\n", governor);
    strncpy(current_policy, governor, BUF_SIZE - 1);
    current_policy[BUF_SIZE - 1] = '\0';

}

void set_turbo(int enable) {
    const char *paths[] = {
        "/sys/devices/system/cpu/intel_pstate/no_turbo",
        "/sys/devices/system/cpu/cpufreq/boost"
    };
    char val[2];
    snprintf(val, sizeof(val), "%d", enable ? 0 : 1); // note inverted logic

    for (int i = 0; i < sizeof(paths)/sizeof(paths[0]); i++) {
        if (access(paths[i], W_OK) == 0) {
            safe_write(paths[i], val);
            turbo_enabled = enable;
            printf("[cpu_manager] Turbo mode %s via %s\n", enable ? "enabled" : "disabled", paths[i]);
            return;
        }
    }
    fprintf(stderr, "[cpu_manager] Turbo control not available on this system\n");
}

void *manager_thread(void *arg) {
    while (runner) {
        // Burada AI logic'i yerleştirilebilir:
        // Örnek: CPU yüküne göre governor veya turbo ayarlarını değiştir.

        // Şimdilik sadece örnek amaçlı basit bir bekleme
        sleep(5);
    }
    return NULL;
}

int init_cpufreg_inline(void) {
    pthread_t thread_id;
    char input[BUF_SIZE];

    printf("[cpu_manager] Advanced CPU Manager started\n");
    printf("Commands:\n  turbo_on\n  turbo_off\n  policy:<governor>\n  exit\n");

    pthread_create(&thread_id, NULL, manager_thread, NULL);

    while (runner) {
        printf("> ");
        fflush(stdout);
        if (!fgets(input, sizeof(input), stdin))
            break;

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "turbo_on") == 0) {
            set_turbo(1);
        } else if (strcmp(input, "turbo_off") == 0) {
            set_turbo(0);
        } else if (strncmp(input, "policy:", 7) == 0) {
            set_governor_all(input + 7);
        } else if (strcmp(input, "exit") == 0) {
            runner = 0;
        } else {
            printf("Unknown command\n");
        }
    }

    pthread_join(thread_id, NULL);
    printf("[cpu_manager] Exiting\n");
    return 0;
}
