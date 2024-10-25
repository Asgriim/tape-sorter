#include <gtest/gtest.h>
#include "file_tape.hpp"
#include <fstream>
#include "fcntl.h"
#include <filesystem>
#include "tape_sorter.hpp"

struct SortTapeEnv : public testing::Test {
    tape::Config config;
    int32_t inpFd, outFd;
    int64_t tapeFileSz;

    std::string testDir = TEST_DATA_DIR;
    std::string inpFile;
    std::string outFile;

    std::vector<int32_t> sorted;

    SortTapeEnv() {
        config.inMemoryLimit = 3;

        inpFile = testDir + "tape_100";
        std::ifstream ifstream(inpFile, std::ios::binary);

        while (!ifstream.eof()) {
            int32_t t;
            if (ifstream.read(reinterpret_cast<char *>(&t), sizeof(int32_t))) {
                sorted.push_back(t);
            }
        }

        std::sort(sorted.begin(), sorted.end());

        inpFd = open(inpFile.data(), O_RDWR);

        tapeFileSz = std::filesystem::file_size(inpFile);

        outFile = testDir + "tmp_sort";
        outFd = open(outFile.data(), O_RDWR | O_CREAT, S_IRWXU);
        ftruncate(outFd, tapeFileSz);
    }

    ~SortTapeEnv() override {
        std::filesystem::remove(outFile);
    }

};


TEST_F(SortTapeEnv, SortTest) {
    tape::Sorter<int32_t> sorter(config);

    tape::FileTape<int32_t> inTape(inpFd, tapeFileSz, config);
    tape::FileTape<int32_t> outTape(outFd, tapeFileSz, config);

    sorter.mergeSort(inTape, outTape);

    while (outTape.prev()) {

    }

    for (int32_t elem : sorted) {
        auto actual = outTape.read();
        EXPECT_EQ(elem, actual);
        outTape.next();
    }
}