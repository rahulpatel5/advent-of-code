#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "input.txt" };

    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };

    auto countLeftBracket{ helper::getNumberOfChars(input, '(') };
    auto countRightBracket{ helper::getNumberOfChars(input, ')') };
    auto floor{ countLeftBracket - countRightBracket };
    std::cout << "On floor " << floor << '\n';
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}
