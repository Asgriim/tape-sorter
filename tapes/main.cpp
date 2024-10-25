#include <iostream>
#include <filesystem>
#include "config/config.hpp"
#include "tape_sorter.hpp"

int main(int argc, char *argv[]) {

    std::string inFile, outFile, confFile;
    int64_t opt = 0;

    while (opt != -1) {
        opt = getopt(argc, argv, "i:o:c:");
        switch (opt) {
            case 'i': {
                inFile = optarg;
                break;
            }
            case 'o': {
                outFile = optarg;
                break;
            }
            case 'c': {
                confFile = optarg;
                break;
            }
        }
    }

    if (inFile.empty() || outFile.empty()) {
        std::cerr << "ERROR: too few arguments \n expected use:\n tape-sorter -i=inp_tape -o=out_tape -c=config.txt\n" ;
        return -1;
    }

    if (!std::filesystem::exists(inFile)) {
        std::cerr << "input tape file not found\n";
        return -1;
    }

    tape::Config config = tape::read_conf(confFile);

    int64_t fileSz = std::filesystem::file_size(inFile);
    int32_t fdIn = open(inFile.data(), O_RDWR);

    int32_t fdOut = open(inFile.data(), O_RDWR | O_CREAT, S_IRWXU);
    ftruncate(fdOut, fileSz);

    tape::FileTape<int32_t> inTape(fdIn, fileSz, config);
    tape::FileTape<int32_t> outTape(fdOut, fileSz, config);

    tape::Sorter<int32_t> sorter(config);
    sorter.mergeSort(inTape, outTape);

    return 0;
}