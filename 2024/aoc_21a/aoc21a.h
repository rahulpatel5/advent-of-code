#ifndef AOC21A_H
#define AOC21A_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include "../../shared/Direction.h"
#include "../../shared/Array.h"
#include <map>
#include <stdexcept>

#include <iostream>

/*
read codes
set up numeric keypad and directional keypad
set up initial position for each keypad
find way to relate starting position to ending position and keeping track
add layers for each keypad
get solution (code complexity)

traverse keypads in a consistent direction (e.g. vertical then horizontal)
this may make things easier?

puzzle mentions getting a shortest sequence of presses
I don't understand if I'm missing some complexity here

I see the complexity: we start on 'A' next to up and right
we want to avoid moving left first, as that takes longer
can we re-organise sequences to prefer right and up?

it would help if I didn't add unnecessary complexity with an extra keypad

account for instruction that we can't hover over where there's no button

getting answer too high
might need to try different iterations to get the shortest sequence?
test producing all iterations and getting the shortest
*/

using Button = std::pair<size_t, size_t>;
using KeypadNum = Array::Array2d<char, 4, 3>;
using KeypadDir = Array::Array2d<char, 2, 3>;
using KeypadMap = std::map<char, Button>;

namespace Numeric
{
    constexpr Button start {2, 3}; // starts on 'A'
    constexpr KeypadNum keypad {{
        {'7', '8', '9'},
        {'4', '5', '6'},
        {'1', '2', '3'},
        {' ', '0', 'A'}
    }};
    KeypadMap keypadPos {
        {'7', {0, 0}},
        {'8', {1, 0}},
        {'9', {2, 0}},
        {'4', {0, 1}},
        {'5', {1, 1}},
        {'6', {2, 1}},
        {'1', {0, 2}},
        {'2', {1, 2}},
        {'3', {2, 2}},
        {'0', {1, 3}},
        {'A', {2, 3}}
    };
}

namespace Directional
{
    constexpr Button start {2, 0}; // starts on 'A'
    constexpr KeypadDir keypad {{
        {' ', '^', 'A'},
        {'<', 'v', '>'}
    }};
    KeypadMap keypadPos {
        {'^', {1, 0}},
        {'A', {2, 0}},
        {'<', {0, 1}},
        {'v', {1, 1}},
        {'>', {2, 1}}
    };
}

namespace aoc21a
{
    using CodeInt = long long;
    using Move = std::pair<int, int>;
    using Moves = std::vector<Move>;
    using Press = char;
    using Presses = std::string;
    using PressList = std::vector<Presses>;
    using PressSequence = std::vector<char>;
    using Movement = std::pair<Move, int>;

    Move getDirection(char c)
    {
        switch (c)
        {
        case '^': return Direction::north;
        case '>': return Direction::east;
        case 'v': return Direction::south;
        case '<': return Direction::west;
        default : throw std::invalid_argument("Given an arrow direction that doesn't exist.\n");
        }
    }

    Press getArrow(Move move)
    {
        if (move == Direction::north)
            return '^';
        else if (move == Direction::east)
            return '>';
        else if (move == Direction::south)
            return 'v';
        else if (move == Direction::west)
            return '<';
        else
            throw std::invalid_argument("Given a direction that doesn't exist.\n");
    }

    Movement getHorizontalMove(Button start, Button end)
    {
        if (end.first > start.first)
            return {Direction::east, end.first - start.first};
        else if (end.first < start.first)
            return {Direction::west, start.first - end.first};
        // no horizontal movement
        else
            return {{0, 0}, 0};
    }

    Movement getVerticalMove(Button start, Button end)
    {
        if (end.second > start.second)
            return {Direction::south, end.second - start.second};
        else if (end.second < start.second)
            return {Direction::north, start.second - end.second};
        // no vertical movement
        else
            return {{0, 0}, 0};
    }

    bool isHorizontalFaster(const Movement& horizontal, const Movement& vertical, Button start)
    {
        // if not moving horizontally or vertically, it doesn't matter
        if (horizontal.second == 0 || vertical.second == 0)
            return true;
        int xDistance {horizontal.first.first * horizontal.first.first * horizontal.second};
        int yDistance {vertical.first.second * vertical.first.second * vertical.second};
        return xDistance < yDistance;
    }

    void addToSequence(PressSequence& presses, const Movement& movement)
    {
        for (int n{0}; n < movement.second; ++n)
            presses.push_back(getArrow(movement.first));
    }

    PressSequence getNumericalPresses(Button start, Button end, bool preferHorizontal)
    {
        PressSequence presses {};
        Movement horizontal {getHorizontalMove(start, end)};
        Movement vertical {getVerticalMove(start, end)};
        // we need to avoid the empty button space
        if (start.first == 0 && end.second == Numeric::keypad.size() - 1)
        {
            addToSequence(presses, horizontal);
            addToSequence(presses, vertical);
        }
        else if (start.second == Numeric::keypad.size() - 1 && end.first == 0)
        {
            addToSequence(presses, vertical);
            addToSequence(presses, horizontal);
        }
        // shortest sequence needs accounting for distance between buttons
        // else if (isHorizontalFaster(horizontal, vertical, start))
        else if (preferHorizontal)
        {
            addToSequence(presses, horizontal);
            addToSequence(presses, vertical);
        }
        else
        {
            addToSequence(presses, vertical);
            addToSequence(presses, horizontal);
        }
        return presses;
    }

    PressSequence getVerticalPresses(Button start, Button end)
    {
        PressSequence presses {};
        Movement horizontal {getHorizontalMove(start, end)};
        Movement vertical {getVerticalMove(start, end)};
        // we need to avoid the empty button space
        if (start.second == 0 && end.first == 0)
        {
            addToSequence(presses, vertical);
            addToSequence(presses, horizontal);
        }
        else if (start.first == 0 && end.second == 0)
        {
            addToSequence(presses, horizontal);
            addToSequence(presses, vertical);
        }
        // shortest sequence needs accounting for distance between buttons
        else if (isHorizontalFaster(horizontal, vertical, start))
        {
            addToSequence(presses, horizontal);
            addToSequence(presses, vertical);
        }
        else
        {
            addToSequence(presses, vertical);
            addToSequence(presses, horizontal);
        }
        return presses;
    }

    PressSequence getNumericSequence(const Presses& code, bool preferHorizontal)
    {
        PressSequence sequence {};
        Button position {Numeric::start};
        for (Press button : code)
        {
            // first move to the new button
            Button next {Numeric::keypadPos.at(button)};
            if (next != position)
            {
                PressSequence arrows {getNumericalPresses(position, next, preferHorizontal)};
                sequence.insert(sequence.end(), arrows.begin(), arrows.end());
            }
            position = next;
            // now press the button
            sequence.push_back('A');
        }
        return sequence;
    }

    PressSequence getDirectionalSequence(const PressSequence& prevSeq)
    {
        PressSequence sequence {};
        Button position {Directional::start};
        for (Press button : prevSeq)
        {
            // first move to the new button
            Button next {Directional::keypadPos.at(button)};
            if (next != position)
            {
                PressSequence arrows {getVerticalPresses(position, next)};
                sequence.insert(sequence.end(), arrows.begin(), arrows.end());
            }
            position = next;
            // now press the button
            sequence.push_back('A');
        }
        return sequence;
    }

    CodeInt getButtonPresses(const Presses& code, bool preferHorizontal)
    {
        // start with numeric keypad
        PressSequence numericSequence {getNumericSequence(code, preferHorizontal)};
        std::cout << code << " (n): ";
        for (auto s : numericSequence)
            std::cout << s;
        std::cout << '\n';
        // move onto first directional keypad
        PressSequence directionalSeq1 {getDirectionalSequence(numericSequence)};
        std::cout << code << " (d1): ";
        for (auto s : directionalSeq1)
            std::cout << s;
        std::cout << '\n';
        // move onto second directional keypad
        PressSequence directionalSeq2 {getDirectionalSequence(directionalSeq1)};
        std::cout << code << " (d2): ";
        for (auto s : directionalSeq2)
            std::cout << s;
        std::cout << '\n';
        return directionalSeq2.size();
    }

    CodeInt getNumericPartOfCode(const Presses& code)
    {
        return std::stoll(code.substr(0, code.size() - 1));
    }

    CodeInt getCodeComplexity(const PressList& sequences)
    {
        CodeInt complexity {0};
        for (const Presses& code : sequences)
        {
            // CodeInt buttonPresses {getButtonPresses(code)};
            CodeInt buttonPresses {};
            bool preferHorizontal {true};
            CodeInt horizontalPresses {getButtonPresses(code, preferHorizontal)};
            preferHorizontal = false;
            CodeInt verticalPresses {getButtonPresses(code, preferHorizontal)};
            if (horizontalPresses < verticalPresses)
                buttonPresses = horizontalPresses;
            else
                buttonPresses = verticalPresses;
            complexity += buttonPresses * getNumericPartOfCode(code);
        }
        return complexity;
    }

    template <std::size_t N>
    CodeInt parseAndGetCodeComplexity(const std::array<std::string_view, N>& lines)
    {
        PressList sequences {};
        for (std::string_view line : lines)
            sequences.push_back(std::string(line));
        return getCodeComplexity(sequences);
    }
}

#endif // AOC21A_H
