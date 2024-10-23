
#include "file_tape.hpp"
#include <filesystem>
#include <iostream>
#include <fcntl.h>
#include <random>
#include <fstream>

int main(int argc, char *argv[]) {
//    std::string_view file = "/home/asgrim/CLionProjects/tape/tape1";
//    uint64_t fileSz = std::filesystem::file_size(file);
//    int32_t fd = open(file.data(), O_RDWR);
//    std::cout << "fd : " << fd << " sz : " << fileSz << "\n";
//    tape::FileTape<int32_t> fileTape(fd, fileSz);
//
//    do {
//        std::cout << fileTape.read() << "\n";
//    } while (fileTape.next());
//
//    fileTape.reverse();
//
//    std::cout << "reversed" << "\n";
//
//    do {
//        std::cout << fileTape.read() << "\n";
//    } while (fileTape.next());
//
//    fileTape.reverse();
//    std::cout << "reversed 2" << "\n";

    std::random_device rd;

    // Initialize the random number generator (Mersenne Twister engine)
    std::mt19937 gen(rd());

    // Define the range for random integers (e.g., between 1 and 100)
    std::uniform_int_distribution<> distr(-1000, 1000);
    std::ofstream ofstream("/home/asgrim/CLionProjects/tape/test/test_data/tape_100.txt");

    // Generate and print 5 random integers
    for (int i = 0; i < 100; ++i) {
        int random_number = distr(gen);
        ofstream << random_number << "\n";
    }


    return 0;
}