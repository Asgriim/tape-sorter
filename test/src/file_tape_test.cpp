#include <gtest/gtest.h>
#include "file_tape.hpp"
#include <fstream>
#include "fcntl.h"
#include <filesystem>

struct FileTapeEnv : public testing::Test {
    std::vector<int32_t> vector;
    tape::Config config;

    FileTapeEnv() {
        std::string dir = TEST_DATA_DIR;
        std::string tapePath = dir + "tape_100";

        std::ifstream ifstream(tapePath, std::ios::binary);

        while (!ifstream.eof()) {
            int32_t t;
            if (ifstream.read(reinterpret_cast<char *>(&t), sizeof(int32_t))) {
                vector.push_back(t);
            }
        }
    }

    tape::FileTape<int32_t> createFileTape() const {
        std::string dir = TEST_DATA_DIR;
        std::string binTape = dir + "tape_100";
        int32_t fd = open(binTape.data(), O_RDWR);

        tape::FileTape<int32_t> fileTape(fd, std::filesystem::file_size(binTape), config);

        return fileTape;
    }
};


TEST_F(FileTapeEnv, TapeReadTest) {
    tape::FileTape<int32_t> fileTape = createFileTape();

    for (auto it = vector.begin(), end = vector.end(); it != end; ++it) {
        ASSERT_EQ(*it, fileTape.read());
        fileTape.next();
    }
}

TEST_F(FileTapeEnv, TapeReverseReadTest) {
    tape::FileTape<int32_t> fileTape = createFileTape();

    while (!fileTape.end()) {
        fileTape.next();
    }

    fileTape.reverse();

    for (auto it = vector.rbegin(), end = vector.rend(); it != end; ++it) {
        ASSERT_EQ(*it, fileTape.read());
        fileTape.next();
    }
}

TEST_F(FileTapeEnv, TapeWriteTest) {
    tape::FileTape<int32_t> fileTape = createFileTape();
    auto old = fileTape.read();
    int32_t newVal = 228;
    fileTape.write(newVal);
    EXPECT_EQ(fileTape.read(), newVal);
    fileTape.write(old);
}