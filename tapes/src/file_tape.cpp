#include "file_tape.hpp"
#include <filesystem>


tape::MmapFileHandle::MmapFileHandle(std::string_view fileName, uint64_t fileLength) : length(fileLength) {
    fd = open(fileName.data(), flags, perms);
    ftruncate(fd, fileLength);
    mmapAddr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (fd == -1 || mmapAddr == MAP_FAILED) {
        throw std::runtime_error("Failed to initialize File tape\n");
    }
    
}

tape::MmapFileHandle::MmapFileHandle(tape::MmapFileHandle &&other) noexcept : fd(other.fd), mmapAddr(other.mmapAddr), length(other.length)  {
    other.fd = -1;
    other.mmapAddr = nullptr;
    other.length = 0;
}

tape::MmapFileHandle &tape::MmapFileHandle::operator=(tape::MmapFileHandle &&other) noexcept {
    if (this != &other) {

        fd = other.fd;
        mmapAddr = other.mmapAddr;
        length = other.length;

        other.fd = -1;
        other.mmapAddr = nullptr;
        other.length = 0;
    }
    return *this;
}

tape::MmapFileHandle::~MmapFileHandle() {
    if (mmapAddr) {
        munmap(mmapAddr, length);
    }
    if (fd > 0) {
        close(fd);
    }
}


