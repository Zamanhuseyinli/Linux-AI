#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cstdio>

std::string detect_gpu_vendor() {
    std::string vendor = "unknown";
    FILE* pipe = popen("lspci | grep VGA", "r");
    if (!pipe) return vendor;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        std::string line(buffer);
        if (line.find("AMD") != std::string::npos || line.find("Advanced Micro Devices") != std::string::npos)
            vendor = "amd";
        else if (line.find("Intel") != std::string::npos)
            vendor = "intel";
        else if (line.find("NVIDIA") != std::string::npos)
            vendor = "nvidia";
    }
    pclose(pipe);
    return vendor;
}

void read_amd_metrics() {
    std::ifstream file("/sys/class/drm/card0/device/gpu_busy_percent");
    std::ifstream memfile("/sys/class/drm/card0/device/mem_info_vram_used");

    int busy = -1;
    unsigned long used_vram = 0;

    if (file.is_open()) {
        file >> busy;
        file.close();
    }
    if (memfile.is_open()) {
        memfile >> used_vram;
        used_vram = used_vram / (1024 * 1024); // Convert to MB
        memfile.close();
    }

    std::cout << "{\n";
    std::cout << "  \"gpu_vendor\": \"amd\",\n";
    std::cout << "  \"gpu_busy_percent\": " << busy << ",\n";
    std::cout << "  \"vram_used_mb\": " << used_vram << "\n";
    std::cout << "}" << std::endl;
}

void read_intel_metrics() {
    FILE* pipe = popen("intel_gpu_top -J -s 200 -o - | head -n 20", "r");
    if (!pipe) {
        std::cerr << "Intel GPU monitoring tool not found.\n";
        return;
    }

    char buffer[512];
    std::string output = "";

    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    pclose(pipe);

    std::cout << "{\n";
    std::cout << "  \"gpu_vendor\": \"intel\",\n";
    std::cout << "  \"metrics_raw\": \"" << output << "\"\n";
    std::cout << "}" << std::endl;
}

void read_nvidia_metrics() {
    std::cerr << "[!] NVIDIA GPU detected but `nvidia-smi` not available or unsupported in this mode.\n";
}

int main() {
    std::string vendor = detect_gpu_vendor();

    if (vendor == "amd") {
        read_amd_metrics();
    } else if (vendor == "intel") {
        read_intel_metrics();
    } else if (vendor == "nvidia") {
        read_nvidia_metrics();
    } else {
        std::cerr << "No supported GPU detected.\n";
    }

    return 0;
}
