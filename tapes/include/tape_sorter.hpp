#ifndef TAPE_TAPE_SORTER_HPP
#define TAPE_TAPE_SORTER_HPP

#include <queue>
#include <algorithm>
#include "i_tape.hpp"
#include "config/config.hpp"


namespace tape {

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
    using TapeVector = std::vector<std::unique_ptr<ITape<T>>>;


    template <typename T>
    class Sorter {
    public:
        explicit Sorter(const Config &config) : m_config(config) {}

        virtual ~Sorter() {

        }

        void mergeSort(ITape<T> *inp, ITape<T> *out) {

            uint64_t memLimit = m_config.inMemoryLimit;

            if (memLimit > inp->length()) {
                memLimit = inp->length();
            }

            auto sortedChunks = slice(inp, memLimit);
            merge(sortedChunks, out);
        }


    private:
       MinHeap<T> initMinHeap(TapeVector<T> &sortedChunks) {
            MinHeap<T> minHeap;
            for (int i = 0, sz = sortedChunks.size(); i < sz; ++i) {
                minHeap.emplace(sortedChunks[i]->read(), i);
            }
            return minHeap;
        }


        void merge(TapeVector<T> &sortedChunks, ITape<T> *out) {
            MinHeap<T> minHeap = initMinHeap(sortedChunks);
            while (!minHeap.empty() && !out->end()) {
                auto minNode = minHeap.top();
                minHeap.pop();

                out->write(minNode.val);
                out->next();

                int32_t minTapeInd = minNode.tapeInd;
                sortedChunks[minTapeInd]->next();

                if (!sortedChunks[minTapeInd]->end()) {
                    minHeap.emplace(sortedChunks[minTapeInd]->read(), minTapeInd);
                }
            }
        }

        TapeVector<T> slice(ITape<T> *inp, const uint64_t limit) {
            TapeVector<T> sortedTapes;

            while (!inp->end()) {
                std::vector<T> buf;
                fillBuffer(inp, buf, limit);
                std::sort(buf.begin(), buf.end());
                auto tmpTape = createTmpTape(buf.size());
                writeBufferToTape(buf, tmpTape.get());
                tmpTape->reverse();

                sortedTapes.emplace_back(std::move(tmpTape));
            }

            return sortedTapes;
        }


        //write in reverse to avoid rewind
        void writeBufferToTape(const std::vector<T> &buff, ITape<T> *tape) {
            for (auto it = buff.rbegin(), end = buff.rend(); it != end && !tape->end(); ++it) {
                tape->write(*it);
                tape->next();
            }
        }

        void fillBuffer(ITape<T> *inp, std::vector<T> &buff, uint64_t limit) {
            while (!inp->end() && buff.size() < limit) {
                buff.push_back(inp->read());
                inp->next();
            }
        }

        virtual std::unique_ptr<ITape<T>> createTmpTape(const uint64_t length) = 0;

    protected:
            Config m_config;
    };
}
#endif //TAPE_TAPE_SORTER_HPP
