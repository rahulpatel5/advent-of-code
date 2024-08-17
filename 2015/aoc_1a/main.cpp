#include <chrono>
#include <iostream>

int main()
{
    auto startTime{ std::chrono::high_resolution_clock::now() };
    
    
    
    auto stopTime{ std::chrono::high_resolution_clock::now() };
    const std::chrono::duration<double> durationSeconds{ stopTime - startTime };
    std::cout << "Time taken: " << durationSeconds.count() << " s\n";
    
    return 0;
}
