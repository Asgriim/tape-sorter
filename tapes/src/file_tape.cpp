#include "file_tape.hpp"

tape::MmapFileHandle::MmapFileHandle(int32_t fd, uint64_t length) : fd(fd), length(length) {
    mmapAddr = mmap(nullptr, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (mmapAddr == MAP_FAILED) {
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
