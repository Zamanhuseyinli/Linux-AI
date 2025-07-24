#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEV_PATH "/dev/ai_lfs"
#define PROC_PATH "/proc/sentielcpu_io_stats"
#define BUFFER_SIZE 128 // 128 bytes for section size or /proc/AI_lifer

int main() {
    char write_data[] = "AI Decision: optimize_io\n";
    char read_buffer[BUFFER_SIZE];

    // /dev/ai_lfs aygıtına yaz
    int dev_fd = open(DEV_PATH, O_WRONLY);
    if (dev_fd < 0) {
        perror("open /dev/ai_lfs");
        return 1;
    }
    write(dev_fd, write_data, strlen(write_data));
    close(dev_fd);

    // /dev/ai_lfs aygıtından oku
    dev_fd = open(DEV_PATH, O_RDONLY);
    if (dev_fd < 0) {
        perror("open /dev/ai_lfs");
        return 1;
    }
    int n = read(dev_fd, read_buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        read_buffer[n] = '\0';
        printf("AI buffer: %s\n", read_buffer);
    }
    close(dev_fd);

    // /proc/sentielcpu_io_stats dosyasını oku
    int proc_fd = open(PROC_PATH, O_RDONLY);
    if (proc_fd < 0) {
        perror("open /proc/sentielcpu_io_stats");
        return 1;
    }
    n = read(proc_fd, read_buffer, BUFFER_SIZE - 1);
    if (n > 0) {
        read_buffer[n] = '\0';
        printf("Kernel IO Stats: %s\n", read_buffer);
    }
    close(proc_fd);

    return 0;
}