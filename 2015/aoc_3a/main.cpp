#include "aoc3a.h"
#include "../../shared/utils.h"
#include <chrono>
#include <iostream>
#include <fstream>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    std::ifstream file{ "input.txt" };
    
    utils::CheckFileError(file);
    std::string input{ utils::ReadFile(file) };
    
    int housesVisited{ aoc3a::housesVisited(input) };
    std::cout << "Houses visited = " << housesVisited << '\n';
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}
