
#include "file_tape.hpp"
#include <filesystem>
#include <iostream>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    std::string_view file = "/home/asgrim/CLionProjects/tape/tape1";
    uint64_t fileSz = std::filesystem::file_size(file);
    int32_t fd = open(file.data(), O_RDWR);
    std::cout << "fd : " << fd << " sz : " << fileSz << "\n";
    tape::FileTape<int32_t> fileTape(fd, fileSz);

    do {
        std::cout << fileTape.read() << "\n";
    } while (fileTape.next());

    fileTape.reverse();

    std::cout << "reversed" << "\n";

    do {
        std::cout << fileTape.read() << "\n";
    } while (fileTape.next());

    fileTape.reverse();
    std::cout << "reversed 2" << "\n";


    return 0;
}