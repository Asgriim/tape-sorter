//
// Created by asgrim on 24.10.24.
//

#ifndef TAPE_TAPE_SORTER_HPP
#define TAPE_TAPE_SORTER_HPP

#include <fcntl.h>
#include <queue>
#include <algorithm>
#include "file_tape.hpp"
#include "config/config.hpp"


namespace tape {
    namespace fs = std::filesystem;

    template <typename T>
    struct HeapNode final {
        T val;
        int32_t tapeInd;

        HeapNode(T val, int32_t tapeInd) : val(val), tapeInd(tapeInd) {}

        bool operator>(const HeapNode& other) const {
            return val > other.val;
        }
    };

    template <typename T>
    using MinHeap = std::priority_queue<HeapNode<T>, std::vector<HeapNode<T>>, std::greater<HeapNode<T>>>;

    template <typename T>
    class Sorter {
    public:
        explicit Sorter(const Config &config) : m_config(config) {}

        void mergeSort(FileTape<T> &inp, FileTape<T> &out) {

            fs::create_directory(m_tempDir);

            uint64_t memLimit = m_config.inMemoryLimit;

            if (memLimit > inp.length()) {
                memLimit = inp.length();
            }

            auto sortedChunks = slice(inp, memLimit);
            merge(sortedChunks, out);

            clear();
        }


    private:
       MinHeap<T> initMinHeap(std::vector<FileTape<T>> &sortedChunks) {
            MinHeap<T> minHeap;
            for (int i = 0, sz = sortedChunks.size(); i < sz; ++i) {
                minHeap.emplace(sortedChunks[i].read(), i);
            }
            return minHeap;
        }


        void merge(std::vector<FileTape<T>> &sortedChunks, FileTape<T> &out) {
            MinHeap<T> minHeap = initMinHeap(sortedChunks);
            while (!minHeap.empty() && !out.end()) {
                auto minNode = minHeap.top();
                minHeap.pop();

                out.write(minNode.val);
                out.next();

                int32_t minTapeInd = minNode.tapeInd;
                sortedChunks[minTapeInd].next();

                if (!sortedChunks[minTapeInd].end()) {
                    minHeap.emplace(sortedChunks[minTapeInd].read(), minTapeInd);
                }
            }
        }

        std::vector<FileTape<T>> slice(FileTape<T> &inp, const uint64_t limit) {
            std::vector<FileTape<T>> sortedTapes;

            while (!inp.end()) {
                std::vector<T> v;
                fillVector(inp, v, limit);
                std::sort(v.begin(), v.end());
                auto tmpTape = createTmpTape(++m_tmpTapeNumber, v.size());
                writeSortedToTape(v, tmpTape);
                tmpTape.reverse();

                sortedTapes.emplace_back(std::move(tmpTape));
            }

            return sortedTapes;
        }


        //write in reverse to avoid rewind
        void writeSortedToTape(const std::vector<T> &chunk, FileTape<T> &tape) {
            for (auto it = chunk.rbegin(), end = chunk.rend(); it != end && !tape.end(); ++it) {
                tape.write(*it);
                tape.next();
            }
        }

        void fillVector(FileTape<T> &inp, std::vector<T> &v, uint64_t limit) {
            while (!inp.end() && v.size() < limit) {
                v.push_back(inp.read());
                inp.next();
            }
        }

        int32_t createFile(const int32_t number, const int64_t fileSize) {
            std::string tmpFile = m_tempDir.string() + std::to_string(number);
            int32_t fd = open(tmpFile.data(), O_CREAT | O_RDWR, m_perms);
            ftruncate(fd, fileSize);
            return fd;
        }

        FileTape<T> createTmpTape(const int32_t number, const uint64_t length) {
            int64_t fileSize = length * sizeof(T);
            int32_t fd = createFile(number, fileSize);
            FileTape<T> fileTape(fd, fileSize, m_config);
            return fileTape;
        }

        void clear() {
            m_tmpTapeNumber = 0;
            fs::remove_all(m_tempDir);
        }

    private:
        Config m_config;
        uint64_t m_tmpTapeNumber = 0;
        fs::path m_tempDir = "/tmp/tapes/";
        int32_t m_perms = S_IRWXU;
    };
}
#endif //TAPE_TAPE_SORTER_HPP
