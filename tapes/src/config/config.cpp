#include "config/config.hpp"
#include <iostream>
#include "config/config_file.hpp"


tape::Config tape::read_conf(std::string_view path)  {
    std::ifstream ifstream(path.data());
    Config config;
    if (ifstream.is_open()) {
        std::vector<std::string> names = {"read_delay", "write_delay", "shift_delay", "in_memory_limit"};
        CFG::ReadFile(ifstream, names, config.readDelay, config.writeDelay, config.shiftDelay, config.inMemoryLimit);
    }
    return config;
}