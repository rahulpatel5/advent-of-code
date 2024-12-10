#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <fstream>

namespace utils
{
    int CheckFileError(std::ifstream& file)
    {
        if (!file)
        {
            std::cerr << "Error. test_input.txt could not be opened or read.\n";
            return 1;
        }
        return 0;
    }

    std::string ReadFile(std::ifstream& file)
    {
        std::string input( (std::istreambuf_iterator<char>(file)),
    (std::istreambuf_iterator<char>()) );
        return input;
    }
}

#endif
