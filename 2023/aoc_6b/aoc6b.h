#ifndef AOC6B_H
#define AOC6B_H

#include <array>
#include <string_view>
#include <string>
#include <algorithm>

/*
FOR each time from 0:
    IF speed and time would beat the record:
        STORE shortest time

FOR each time going down from max time:
    IF speed and time would beat the record:
        STORE longest time

RETURN the counts from the shortest to the longest times

Logic above is based on product being max for fixed sum when
values are closest together

S = x + y [fixed sum]
x * y [product]
x * (S - x) [substitution]
Sx - x**2

derivative:
S - 2x
set this to 0 to find maximum (technically, extremum)
S - 2x = 0
S = 2x
x = S / 2

S = (S / 2) + y [substitute into original formula]
y = S - S / 2
y = S / 2

x = y, so maximise product if x and y are equal

The rearranged formula (Sx - x**2) is quadratic
So, we know that the shape of the curve is a parabola
It's a negative parabola so it will have a maximum
We're looking for successes above a threshold
So, if we start from the extremes and go up/down, we'll find
the shortest and longest times that succeed
And we can use the approach above to simplify the code logic
*/

namespace aoc6b
{
    using TimeInt = long long;
    template <std::size_t L>
    using Lines = std::array<std::string_view, L>;

    using Distance = long long;

    template <std::size_t N>
    void getRaceInfo(TimeInt& time, Distance& distance, const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            std::string row { std::string(line) };
            size_t index { row.find(':') + 1 };

            std::string info { row.substr(0, index - 1) };

            std::string numberStr { row.substr(index) };
            // remove whitespace to leave the number
            numberStr.erase(std::remove_if(numberStr.begin(), numberStr.end(), isspace), numberStr.end());

            if (info == "Time")
            {
                time = std::stoll(numberStr);
            }
            else if (info == "Distance")
            {
                distance = std::stoll(numberStr);
            }
        }
    }

    TimeInt isRaceWin(TimeInt raceTime, TimeInt time, Distance distance)
    {
        long long speed { raceTime };
        long long remainingTime { time - raceTime };
        
        return (speed * remainingTime) > distance;
    }

    template <std::size_t N>
    TimeInt getTravelTime(const Lines<N>& lines)
    {
        TimeInt time {};
        Distance distance {};
        getRaceInfo(time, distance, lines);

        TimeInt shortestSuccessTime {};
        TimeInt raceTime {0};
        while (raceTime < time / 2)
        {
            if (isRaceWin(raceTime, time, distance))
            {
                shortestSuccessTime = raceTime;
                break;
            }
            ++raceTime;
        }

        TimeInt longestSuccessTime {};
        raceTime = time;
        while (raceTime > time / 2)
        {
            if (isRaceWin(raceTime, time, distance))
            {
                longestSuccessTime = raceTime;
                break;
            }
            --raceTime;
        }

        return longestSuccessTime - (shortestSuccessTime - 1);
    }
}

#endif // AOC6B_H
