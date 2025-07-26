#include "gpu_model_runner.hpp"
#include "gpu-buffer-unit.hpp"

int main() {
    // Initialize GPU model runner first
    gpu_model_runner::init_gpu_model_runner();

    // Then create GPU buffer logger and start command loop
    GpuBuffer logger;
    logger.runCommandLoop();

    return 0;
}
