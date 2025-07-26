#include "cpufreg-inline.h"
#include "cpu_metrics_collector.h"
#include "cpu-sensivity-frogline.h"
#include "sentielcpu_io_stats.h"
#include "cpu-buffer-unit.h"

int main() {
    // Tüm modülleri başlat
    init_cpu_buffer_unit();
    init_cpufreg_inline();
    init_cpu_metrics_collector();
    init_cpu_sensivity_frogline();
    init_sentiel_cpu_io_stats();

    // Burada komut döngüsü veya diğer işlemler

    return 0;
}
