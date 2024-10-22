#include "converter.hpp"
#include "cstdint"
#include <getopt.h>


//todo add options -x from text -t from tape
int main(int argc, char *argv[]) {
    std::string_view errMessage = "ERROR: too few arguments \n expected use:\n converter <opt> <input> <output>\n" ;

    if (argc < 4) {
        std::cerr << errMessage;
        return -1;
    }

    //expected files at the end
    std::string_view inp = argv[argc - 2];
    std::string_view out = argv[argc - 1];

    tape::Converter<int32_t> converter;

    int64_t opt = 0;
    while (opt != -1) {
        opt = getopt(argc, argv, "tx");
        switch (opt) {
            case 't': {
                converter.fromTapeToText(inp, out);
                return 0;
            }
            case 'x': {
                converter.fromTextFileToTapeFile(inp, out);
                return 0;
            }
        }
    }
    std::cerr << errMessage;
    return -1;

}