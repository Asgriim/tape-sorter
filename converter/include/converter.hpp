#ifndef TAPE_CONVERTER_HPP
#define TAPE_CONVERTER_HPP

#include <vector>
#include <string_view>
#include "iostream"
#include "fstream"

namespace tape {
    template <typename T>
    class Converter {
    public:

        void fromVectorToTapeFile(const std::vector<T> &v, std::string_view outFile) {
            std::ofstream ostream;
            ostream.open(outFile.data(), std::ios::out | std::ios::binary);
            for (auto &elem : v) {
                writeToTapeFile(elem, ostream);
            }
        }

        //type T need to have operator >> overloaded
        void fromTextFileToTapeFile(std::string_view inpFile, std::string_view outFile) {
            std::ifstream istream;
            std::ofstream ostream;
            istream.open(inpFile.data());
            ostream.open(outFile.data(), std::ios::out | std::ios::binary);

            T elem;
            while (istream >> elem) {
                writeToTapeFile(elem, ostream);
            }
        }

        void fromTapeToText(std::string_view inpFile, std::string_view outFile) {
            std::ifstream istream;
            std::ofstream ostream;
            istream.open(inpFile.data(), std::ios::in | std::ios::binary);
            ostream.open(outFile.data());

            while (istream) {
                T elem;
                if (istream.read(reinterpret_cast<char *>(&elem), sizeof(T))) {
                    ostream << elem << "\n";
                }
            }
        }

    private:
        void writeToTapeFile(T &elem, std::ostream &ostream) {
            ostream.write(reinterpret_cast<const char *>(&elem), sizeof(T));
        }

    };
}


#endif //TAPE_CONVERTER_HPP
