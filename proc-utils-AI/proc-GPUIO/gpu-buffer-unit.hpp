#ifndef GPU_BUFFER_HPP
#define GPU_BUFFER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <cstring>
#include <ctime>
#include <string>

class GpuBuffer {
public:
    struct Entry {
        timespec timestamp;
        char data[128];
    };

    explicit GpuBuffer(int max_entries = 64, const std::string& config_path = "/var/AI-stump/GPU-runtime.yml")
        : bufferMax(max_entries), writeIndex(0), configPath(config_path), bufferEntries(max_entries)
    {
        loadConfig();
        std::cout << "GPU Buffer Userspace Logger Started [bufferMax=" << bufferMax << "]\n";
    }

    void writeEntry(const std::string& input) {
        std::lock_guard<std::mutex> lock(bufferMutex);

        if (writeIndex >= bufferMax) {
            std::memmove(&bufferEntries[0], &bufferEntries[1], sizeof(Entry) * (bufferMax - 1));
            writeIndex = bufferMax - 1;
        }

        clock_gettime(CLOCK_REALTIME, &bufferEntries[writeIndex].timestamp);
        std::strncpy(bufferEntries[writeIndex].data, input.c_str(), sizeof(bufferEntries[writeIndex].data) - 1);
        bufferEntries[writeIndex].data[sizeof(bufferEntries[writeIndex].data) - 1] = '\0';
        ++writeIndex;
    }

    void printBuffer() const {
        std::lock_guard<std::mutex> lock(bufferMutex);

        for (int i = 0; i < writeIndex; ++i) {
            long ms = bufferEntries[i].timestamp.tv_sec * 1000 + bufferEntries[i].timestamp.tv_nsec / 1000000;
            std::cout << "[" << i << "] (" << ms << " ms) " << bufferEntries[i].data << "\n";
        }
    }

    void changeBufferSize(int new_size) {
        if (new_size < 1 || new_size > 512) {
            std::cout << "Invalid buffer size.\n";
            return;
        }

        std::lock_guard<std::mutex> lock(bufferMutex);
        bufferMax = new_size;
        writeIndex = 0;
        bufferEntries.clear();
        bufferEntries.resize(bufferMax);

        std::cout << "Buffer size changed to " << bufferMax << "\n";
    }

    void runCommandLoop() {
        std::string command;
        while (true) {
            std::cout << "\nCommand (write/print/set/exit): ";
            if (!std::getline(std::cin, command)) break;

            if (command.rfind("write ", 0) == 0) {
                writeEntry(command.substr(6));
            } else if (command == "print") {
                printBuffer();
            } else if (command.rfind("set ", 0) == 0) {
                int new_size = std::stoi(command.substr(4));
                changeBufferSize(new_size);
            } else if (command == "exit") {
                break;
            } else {
                std::cout << "Unknown command.\n";
            }
        }
    }

private:
    int bufferMax;
    int writeIndex;
    std::vector<Entry> bufferEntries;
    std::string configPath;
    mutable std::mutex bufferMutex;

    bool loadConfig() {
        std::ifstream file(configPath);
        if (!file.is_open())
            return false;

        std::string line;
        while (std::getline(file, line)) {
            if (line.find("buffer_size:") != std::string::npos) {
                int val = 0;
                std::istringstream iss(line.substr(line.find(":") + 1));
                iss >> val;
                if (val >= 1 && val <= 512) {
                    std::lock_guard<std::mutex> lock(bufferMutex);
                    bufferMax = val;
                    bufferEntries.resize(bufferMax);
                }
            }
        }
        return true;
    }
};

#endif // GPU_BUFFER_HPP
