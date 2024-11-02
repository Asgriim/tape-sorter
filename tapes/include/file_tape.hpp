
#ifndef TAPE_FILE_TAPE_HPP
#define TAPE_FILE_TAPE_HPP

#include <unistd.h>
#include <sys/mman.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <cstring>
#include <chrono>
#include <thread>
#include <fcntl.h>
#include "i_tape.hpp"
#include "config/config.hpp"


namespace tape {
    namespace fs = std::filesystem;
    namespace thread = std::this_thread;

    using ms = std::chrono::milliseconds;

    struct MmapFileHandle {
        MmapFileHandle(std::string_view fileName, uint64_t fileLength);
        ~MmapFileHandle();

        MmapFileHandle(const MmapFileHandle&) = delete;
        MmapFileHandle& operator=(const MmapFileHandle&) = delete;

        MmapFileHandle(MmapFileHandle&& other) noexcept;
        MmapFileHandle& operator=(MmapFileHandle&& other) noexcept;

        int32_t fd = 0;
        void *mmapAddr = nullptr;
        uint64_t length = 0;
        int32_t flags = O_CREAT | O_RDWR;
        int32_t perms = S_IRWXU ;
    };


    template <typename T>

    class FileTape final : public ITape<T> {
        static_assert(std::is_trivially_copyable_v<T>);
    public:


        explicit FileTape(std::string_view fileName, uint64_t length, const Config &config) :
                                                                            m_length(length),
                                                                            m_fileHandle(fileName, length * sizeof (T)),
                                                                            m_config(config) {
            m_startPtr = reinterpret_cast<char*>(m_fileHandle.mmapAddr);
            m_endPtr = m_startPtr + m_fileHandle.length;
            m_currPtr = m_startPtr;
        }

        T read() override {
            if (end()) {
                throw std::runtime_error("File tape read out of bounds");
            }

            thread::sleep_for(ms(m_config.readDelay));

            T elem;
            std::memcpy(reinterpret_cast<char*>(&elem), m_currPtr, sizeof(T));
            return elem;
        }

        void write(const T &elem) override {
            if (end()) {
                throw std::runtime_error("File tape write out of bounds");
            }

            thread::sleep_for(ms(m_config.writeDelay));

            std::memcpy(m_currPtr, reinterpret_cast<const char*>(&elem), sizeof(T));
        }

        bool prev() override {

            if (m_currPtr == m_startPtr) {
                return false;
            }

            thread::sleep_for(ms(m_config.shiftDelay));

            m_currPtr -= m_offset;
            return true;
        }

        bool next() override {
            char *next = m_currPtr + m_offset ;
            if (end()) {
                return false;
            }

            thread::sleep_for(ms(m_config.shiftDelay));

            m_currPtr = next;
            return true;
        }

        bool end() override {
            return m_currPtr == m_endPtr;
        }

        void reverse() override {
            //just like in std::vector:
            // m_endPtr points to to the element right after the last
            // we need to maintain this after reverse
            if (end()) {
                m_currPtr -= m_offset;
            }

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
        Config m_config;
        int64_t m_offset = sizeof(T);
        uint64_t m_length;
        char *m_startPtr;
        char *m_currPtr;
        char *m_endPtr;
    };
}


#endif //TAPE_FILE_TAPE_HPP
