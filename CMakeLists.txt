cmake_minimum_required(VERSION 3.10)
project(proc_utils_AI)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Ortak derleyici bayrakları
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wall -I. -I/usr/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -I. -I/usr/include")

# Kaynak dizinleri
set(CPU_SRC     ${CMAKE_SOURCE_DIR}/proc-utils-AI/proc-CPUIO)
set(GPU_SRC     ${CMAKE_SOURCE_DIR}/proc-utils-AI/proc-GPUIO)
set(KERNEL_SRC  ${CMAKE_SOURCE_DIR}/proc-utils-AI/nproc-kernel)

# CPU araç kaynakları
set(CPU_OBJS
    ${CPU_SRC}/cpu-buffer-unit.c
    ${CPU_SRC}/cpufreg-inline.c
    ${CPU_SRC}/cpu_metrics_collector.c
    ${CPU_SRC}/cpu-sensivity-frogline.c
    ${CPU_SRC}/sentielcpu_io_stats.c
)

# GPU araç kaynakları
set(GPU_OBJS
    ${GPU_SRC}/gpu-buffer-unit.c
    ${GPU_SRC}/gpu_model_runner.cpp
)

# Kernel modülü kaynakları
set(KERNEL_OBJS
    ${KERNEL_SRC}/kernel-start.c
)

# Çıktı klasörü
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

# CPU binary
add_executable(cpu_tools ${CPU_OBJS})
set_target_properties(cpu_tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# GPU binary
add_executable(gpu_tools ${GPU_OBJS})
set_target_properties(gpu_tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

# Kernel binary
add_executable(kernel_mod ${KERNEL_OBJS})
set_target_properties(kernel_mod PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
