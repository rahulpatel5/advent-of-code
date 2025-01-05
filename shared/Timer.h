#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer
{
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;

public:
    Timer()
        : begin {std::chrono::high_resolution_clock::now()}
    {}

    void start()
    {
        begin = std::chrono::high_resolution_clock::now();
    }

    double getDuration()
    {
        std::chrono::duration<double> duration {std::chrono::high_resolution_clock::now() - begin};
        return duration.count();
    }
};

#endif // TIMER_H
