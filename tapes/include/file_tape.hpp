
#ifndef TAPE_FILE_TAPE_HPP
#define TAPE_FILE_TAPE_HPP

#include <unistd.h>
#include <sys/mman.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <cstring>
#include "i_tape.hpp"


namespace tape {

    struct MmapFileHandle {
        MmapFileHandle(int32_t fd, uint64_t length);
        ~MmapFileHandle();

        MmapFileHandle(const MmapFileHandle&) = delete;
        MmapFileHandle& operator=(const MmapFileHandle&) = delete;

        MmapFileHandle(MmapFileHandle&& other) noexcept;
        MmapFileHandle& operator=(MmapFileHandle&& other) noexcept;

        int32_t fd = 0;
        void *mmapAddr = nullptr;
        uint64_t length = 0;

    };


    template <typename T>
    class FileTape final : public ITape<T> {
    public:
        static_assert(std::is_trivially_copyable_v<T> == true);
        //todo add config
        //todo add delays
        //File tape takes ownership for file descriptor
        FileTape(const int32_t fd, const uint64_t fileLength) : m_fileHandle(fd, fileLength - (fileLength % sizeof(T)) ) {
            m_length = fileLength / m_offset;

            m_startPtr = reinterpret_cast<char*>(m_fileHandle.mmapAddr);
            m_endPtr = m_startPtr + m_fileHandle.length;
            m_currPtr = m_startPtr;
        }

        T read() override {
            T elem;
            std::memcpy(reinterpret_cast<char*>(&elem), m_currPtr, sizeof(T));
            return elem;
        }

        void write(const T &elem) override {
            std::memcpy(m_currPtr, reinterpret_cast<const char*>(&elem), sizeof(T));
        }

        bool prev() override {

            if (m_currPtr == m_startPtr) {
                return false;
            }
            m_currPtr -= m_offset;
            return true;
        }

        bool next() override {
            char *next = m_currPtr + m_offset ;
            if (next == m_endPtr) {
                return false;
            }
            m_currPtr = next;
            return true;
        }

        void reverse() override {
            //just like in std::vector:
            // m_endPtr points to to the element right after the last
            // we need to maintain this after reverse
            m_startPtr -= m_offset;
            m_endPtr   -= m_offset;

            std::swap(m_startPtr, m_endPtr);
            m_offset *= -1;
        }

        [[nodiscard]]
        uint64_t length() const override {
            return m_length;
        }


    private:
        MmapFileHandle m_fileHandle;
        int64_t m_offset = sizeof(T);
        uint64_t m_length;
        char *m_startPtr;
        char *m_currPtr;
        char *m_endPtr;
    };
}


#endif //TAPE_FILE_TAPE_HPP
