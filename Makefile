# Compiler and flags (use environment variables if set)
CC ?= gcc
CXX ?= g++
CFLAGS ?= -O2 -Wall -I. -I/usr/include -I/usr/include/linux -I/usr/include/linux/arch/x86/include/asm
CXXFLAGS ?= -O2 -Wall -I. -I/usr/include -I/usr/include/linux -I/usr/include/linux/arch/x86/include/asm

# Kernel headers and include
KERNEL_HEADERS ?= /usr/include/
KERNEL_INCLUDE = -I/usr/include/linux \
	-I$(KERNEL_HEADERS)/linux \
	-I$(KERNEL_HEADERS)/uapi \
	-I$(KERNEL_HEADERS)/asm-generic

# Output directory
BIN_DIR ?= bin

# Source directories
CPU_SRC ?= proc-utils-AI/proc-CPUIO
GPU_SRC ?= proc-utils-AI/proc-GPUIO
KERNEL_SRC ?= proc-utils-AI/nproc-kernel

# Source files
CPU_OBJS = $(CPU_SRC)/cpu-buffer-unit.c \
	$(CPU_SRC)/cpufreg-inline.c \
	$(CPU_SRC)/cpu_metrics_collector.c \
	$(CPU_SRC)/cpu-sensivity-frogline.c \
	$(CPU_SRC)/sentielcpu_io_stats.c

GPU_OBJS = $(GPU_SRC)/gpu-buffer-unit.c \
	$(GPU_SRC)/gpu_model_runner.cpp

KERNEL_OBJS = $(KERNEL_SRC)/kernel-start.c

# Targets
all: $(BIN_DIR) $(BIN_DIR)/cpu_tools $(BIN_DIR)/gpu_tools $(BIN_DIR)/kernel_mod

# Create binary directory
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# CPU tools compilation
$(BIN_DIR)/cpu_tools: $(CPU_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(KERNEL_INCLUDE)

# GPU tools compilation
$(BIN_DIR)/gpu_tools: $(GPU_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(KERNEL_INCLUDE)

# Kernel module compilation
$(BIN_DIR)/kernel_mod: $(KERNEL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(KERNEL_INCLUDE)

# Clean
clean:
	rm -rf $(BIN_DIR)
