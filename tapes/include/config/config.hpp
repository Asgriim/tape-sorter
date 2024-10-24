#ifndef TAPE_CONFIG_HPP
#define TAPE_CONFIG_HPP

#include <cstdint>

namespace tape {
    struct Config {
        int32_t readDelay = 0;
        int32_t writeDelay = 0;
        int32_t shiftDelay = 0;
        int32_t inMemoryLimit = INT32_MAX;
    };
}
#endif //TAPE_CONFIG_HPP
