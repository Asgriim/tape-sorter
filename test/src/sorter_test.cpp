#include <gtest/gtest.h>
#include "file_tape.hpp"
#include <fstream>
#include <filesystem>
#include "file_tape_sorter.hpp"

struct SortTapeEnv : public testing::Test {
    tape::Config config;
    int64_t tapeLength = 100;

    std::string testDir = TEST_DATA_DIR;
    std::string inpFile;
    std::string outFile;

    std::vector<int32_t> sorted;

    SortTapeEnv() {
        config.inMemoryLimit = 3;

        inpFile = testDir + "tape_100";
        outFile = testDir + "tmp_sort";
        std::ifstream ifstream(inpFile, std::ios::binary);

        while (!ifstream.eof()) {
            int32_t t;
            if (ifstream.read(reinterpret_cast<char *>(&t), sizeof(int32_t))) {
                sorted.push_back(t);
            }
        }

        std::sort(sorted.begin(), sorted.end());
    }

    ~SortTapeEnv() override {
        std::filesystem::remove(outFile);
    }

};


TEST_F(SortTapeEnv, SortTest) {
    tape::FileTapeSorter<int32_t> sorter(config);

    tape::FileTape<int32_t> inTape(inpFile, tapeLength, config);
    tape::FileTape<int32_t> outTape(outFile, tapeLength, config);

    sorter.mergeSort(&inTape, &outTape);

    while (outTape.prev()) {

    }

    for (int32_t elem : sorted) {
        auto actual = outTape.read();
        EXPECT_EQ(elem, actual);
        outTape.next();
    }
}