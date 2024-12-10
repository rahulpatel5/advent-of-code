#include "../../shared/utils.h"
#include "../../shared/helper.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "../aoc_1a/input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int count{0};
    for (int i{0}; i < input.length(); ++i)
    {
        if (input[i] == '(')
            ++count;
        else if (input[i] == ')')
            --count;
        
        if (count == -1)
        {
            std::cout << "Reached floor -1 at position " << i + 1 << '\n';
            break;
        }
    }
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}
