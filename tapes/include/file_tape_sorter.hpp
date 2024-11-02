
#ifndef README_MD_FILE_TAPE_SORTER_HPP
#define README_MD_FILE_TAPE_SORTER_HPP
#include "tape_sorter.hpp"
#include "file_tape.hpp"
#include <filesystem>
#include "config/config.hpp"

namespace tape {
    template <typename T>
    class FileTapeSorter : public Sorter<T> {
    public:
        explicit FileTapeSorter(const Config &config) : Sorter<T>(config) {
            fs::create_directory(m_tmpDir);
        }

        virtual ~FileTapeSorter() {
            fs::remove_all(m_tmpDir);
        }

    private:
        std::unique_ptr<ITape<T>> createTmpTape(const uint64_t length) override {
            std::string filePath = m_tmpDir.string() + std::to_string(++m_tmpTapeCount);
            auto tmpTape = std::make_unique<FileTape<T>>(filePath, length, Sorter<T>::m_config);
            return tmpTape;
        }
    private:
        fs::path m_tmpDir = "/tmp/tapes/";
        uint64_t m_tmpTapeCount = 0;
    };
}
#endif //README_MD_FILE_TAPE_SORTER_HPP
