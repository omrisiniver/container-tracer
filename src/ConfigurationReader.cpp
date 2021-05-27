#include "ConfigurationReader.hpp"
#include "nholmann/json.hpp"

#include <fstream>
#include <iostream>


using json = nlohmann::json;

constexpr char* PROCESSES        = "processes";
constexpr char* BLOCKED_SYSCALLS = "blocked_syscalls";
constexpr char* SERVER_IP        = "server_ip";
constexpr char* SERVER_PORT      = "server_port";

static void runner(const json& obj, std::function<void(const json&)> f) {
    for (const auto& item : obj) {
        f(item);
    }
}

std::unique_ptr<ConfObject> ConfigurationReader::read_config(const std::string& path) {
    std::ifstream ifs(path);

    if (! ifs) {
        return nullptr;
    }

    json jf;
    try {
        jf = json::parse(ifs);

    } catch (...) {
        std::cout << "Failed parsing JSON file" << std::endl;
        return nullptr;
    }

    auto _ret = std::make_unique<ConfObject>();
    if (! _ret) {
        std::cout << "Failed allocating memory" << std::endl;
        return nullptr;
    }

    if (! jf[PROCESSES].is_array()) {
        std::cout << "Processes must appear in the config" << std::endl;
        return nullptr;
    }

    if (! jf[SERVER_IP].is_string()) {
        std::cout << "Server IP must be a string" << std::endl;
        return nullptr;
    }

    if (! jf[SERVER_PORT].is_number_unsigned()) {
        std::cout << "Server Port must be unsigned" << std::endl;
        return nullptr;
    }

    auto& ret = *_ret;

    ret.server_ip = jf[SERVER_IP];

    if (jf[SERVER_PORT] >= std::numeric_limits<uint16_t>::max()) {
        std::cout << "Invalid Port" << std::endl;
        return nullptr;
    }

    ret.server_port = jf[SERVER_PORT];

    runner(jf[PROCESSES], [&ret](const json& item) {
            if (item.is_string()) {
                ret.process_names.push_back(item);
            }
    });

    runner(jf[BLOCKED_SYSCALLS], [&ret](const json& item) {
        if (! item.is_number_unsigned()) {
                return;
        }

        if (item > std::numeric_limits<uint16_t>::max()) {
                return;
        }

        ret.blocked_syscalls.push_back(item);
    });

    return _ret;
}

