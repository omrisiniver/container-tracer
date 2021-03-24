#pragma once

#include <memory>
#include <vector>

struct ConfObject {
    std::vector<std::string> process_names;
    std::vector<uint16_t> blocked_syscalls;
};

class ConfigurationReader {
public:
    ~ConfigurationReader() = delete;
    
    static std::unique_ptr<ConfObject> read_config(const std::string& path);
};