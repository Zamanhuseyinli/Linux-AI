# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -O2 -Wall -I. -I/usr/include
CXXFLAGS = -O2 -Wall -I.
KERNEL_HEADERS = /lib/modules/$(shell uname -r)/build
KERNEL_INCLUDE = -I$(KERNEL_HEADERS)/include

# Output
BIN_DIR = bin

# Source directories
CPU_SRC = proc-utils-AI/proc-CPUIO
GPU_SRC = proc-utils-AI/proc-GPUIO
KERNEL_SRC = proc-utils-AI/nproc-kernel

# Sources
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

$(BIN_DIR):	
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/cpu_tools: $(CPU_OBJS)	
	$(CC) $(CFLAGS) -o $@ $^ $(KERNEL_INCLUDE)

$(BIN_DIR)/gpu_tools: $(GPU_OBJS)	
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BIN_DIR)/kernel_mod: $(KERNEL_OBJS)	
	$(CC) $(CFLAGS) -o $@ $^ $(KERNEL_INCLUDE)


clean:	
	rm -rf $(BIN_DIR)

.PHONY: all clean nconfig
