//
// Created by asgrim on 22.10.24.
//

#ifndef TAPE_FILE_TAPE_HPP
#define TAPE_FILE_TAPE_HPP

#include <unistd.h>
#include <sys/mman.h>
#include <memory>

#include "i_tape.hpp"


namespace tape {

    //do not access outside
    namespace details {
        struct FileHandle {
            int32_t fd;
            void *mmapAddr;
            uint64_t length;
        };

        struct FileHandleDeleter {
            void operator()(FileHandle *fileHandle) {
                close(fileHandle->fd);
                munmap(fileHandle->mmapAddr, fileHandle->length);
                delete fileHandle;
            }
        };

        using FileHandlePtr = std::unique_ptr<details::FileHandle, details::FileHandleDeleter>;
    }



    template <typename T>
    class FileTape : public ITape<T>{
    public:
        FileTape() : m_fileHandle(new details::FileHandle) {

        }
    private:
        details::FileHandlePtr m_fileHandle;
    };
}


#endif //TAPE_FILE_TAPE_HPP
