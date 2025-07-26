#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stddef.h>

void print_sentielcpu_io_stats(void) {
    uint32_t read_bytes, write_bytes;

    srand((unsigned int)time(NULL));

    read_bytes = rand() % 50000;
    write_bytes = rand() % 50000;

    printf("read: %u\nwrite: %u\n", read_bytes, write_bytes);
}

void init_sentiel_cpu_io_stats() {
    print_sentielcpu_io_stats();
}
