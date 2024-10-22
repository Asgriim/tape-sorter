//
// Created by asgrim on 22.10.24.
//

#ifndef TAPE_I_TAPE_HPP
#define TAPE_I_TAPE_HPP

#include <cstdint>


namespace tape {
    template <typename T>
    class ITape {
    public:
        virtual ~ITape() {}

        virtual T read() = 0;
        virtual void write(const T &elem) = 0;

        virtual bool prev() = 0;
        virtual bool next() = 0;
        // reverse read and write from [end] to -> [start]
        virtual void reverse() = 0;

        // Length in number of elements
        virtual uint64_t length() const = 0;
    };
}

#endif //TAPE_I_TAPE_HPP
