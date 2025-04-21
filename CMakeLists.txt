# CMake minimum required version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(LinuxAI VERSION 1.0)

# Compiler settings
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# Compiler flags
set(CMAKE_C_FLAGS "-O2 -Wall -I. -I/usr/include")
set(CMAKE_CXX_FLAGS "-O2 -Wall -I.")

# Kernel headers location
set(KERNEL_HEADERS "/lib/modules/${CMAKE_SYSTEM_NAME}/build")
include_directories(${KERNEL_HEADERS}/include)

# Output directory for binaries
set(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

# Source directories
set(CPU_SRC proc-utils-AI/proc-CPUIO)
set(GPU_SRC proc-utils-AI/proc-GPUIO)
set(KERNEL_SRC proc-utils-AI/nproc-kernel)

# Sources
set(CPU_OBJS
    ${CPU_SRC}/cpu-buffer-unit.c
    ${CPU_SRC}/cpufreg-inline.c
    ${CPU_SRC}/cpu_metrics_collector.c
    ${CPU_SRC}/cpu-sensivity-frogline.c
    ${CPU_SRC}/sentielcpu_io_stats.c
)

set(GPU_OBJS
    ${GPU_SRC}/gpu-buffer-unit.c
    ${GPU_SRC}/gpu_model_runner.cpp
)

set(KERNEL_OBJS
    ${KERNEL_SRC}/kernel-start.c
)

# Create executable targets
add_executable(cpu_tools ${CPU_OBJS})
add_executable(gpu_tools ${GPU_OBJS})
add_executable(kernel_mod ${KERNEL_OBJS})

# Set the output directory for executables
set_target_properties(cpu_tools gpu_tools kernel_mod PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

# Link the kernel include directory
target_include_directories(cpu_tools PRIVATE ${KERNEL_HEADERS}/include)
target_include_directories(gpu_tools PRIVATE ${KERNEL_HEADERS}/include)
target_include_directories(kernel_mod PRIVATE ${KERNEL_HEADERS}/include)

# Clean command to remove the binaries
add_custom_target(clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${BIN_DIR}
)
