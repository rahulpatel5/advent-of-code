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
#include <algorithm>

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

    void addSequence(Presses& presses, const Movement& movement)
    {
        for (int n{0}; n < movement.second; ++n)
            presses.push_back(getArrow(movement.first));
    }

    PressList getNumericalPresses(Button start, Button end)
    {
        Movement horizontalMove {getHorizontalMove(start, end)};
        Movement verticalMove {getVerticalMove(start, end)};

        Presses presses {};
        addSequence(presses, horizontalMove);
        addSequence(presses, verticalMove);

        // generate iterations for the combined sequence
        PressList sequences {};
        sequences.push_back(presses);
        while (std::next_permutation(presses.begin(), presses.end()))
        {
            sequences.push_back(presses);
        }

        // we need to avoid the empty button space
        if ((start.first == 0 && end.second == Numeric::keypad.size() - 1) || (start.second == Numeric::keypad.size() - 1 && end.first == 0))
        {
            Presses beginSeq;
            // if going left to down right
            if (start.first == 0)
            {
                if (start.second == 0)
                    beginSeq = "vvv";
                else if (start.second == 1)
                    beginSeq = "vv";
                else if (start.second == 2)
                    beginSeq = "v";
            }
            // if going down right to up left
            else if (end.first == 0)
            {
                if (start.first == 1)
                    beginSeq = "<";
                else if (start.first == 2)
                    beginSeq = "<<";
            }

            auto it {std::find_if(sequences.begin(), sequences.end(), [&beginSeq](const std::string& s) {
                return s.find(beginSeq) == 0;
            })};
            if (it != sequences.end())
                sequences.erase(it);
        }
        return sequences;
    }

    PressList getDirectionalPresses(Button start, Button end)
    {
        Movement horizontalMove {getHorizontalMove(start, end)};
        Movement verticalMove {getVerticalMove(start, end)};

        Presses presses {};
        addSequence(presses, horizontalMove);
        addSequence(presses, verticalMove);

        // generate iterations for the combined sequence
        PressList sequences {};
        sequences.push_back(presses);
        while (std::next_permutation(presses.begin(), presses.end()))
        {
            sequences.push_back(presses);
        }

        // we need to avoid the empty button space
        if ((start.second == 0 && end.first == 0) || (start.first == 0 && end.second == 0))
        {
            Presses beginSeq;
            // if going up right to down left
            if (end.first == 0)
            {
                if (start.first == 1)
                    beginSeq = "<";
                else if (start.first == 2)
                    beginSeq = "<<";
            }
            // if going down left to up right
            else if (start.first == 0)
            {
                beginSeq = "^";
            }

            auto it {std::find_if(sequences.begin(), sequences.end(), [&beginSeq](const std::string& s) {
                return s.find(beginSeq) == 0;
            })};
            if (it != sequences.end())
                sequences.erase(it);
        }
        return sequences;
    }

    PressList addNewArrows(const PressList& sequences, const PressList& arrows)
    {
        if (sequences.size() == 0)
            return arrows;

        PressList newSequences {};
        for (const Presses& seq : sequences)
        {
            for (const Presses& newArrows : arrows)
            {
                Presses combined {seq};
                combined.append(newArrows);
                newSequences.push_back(combined);
            }
        }
        return newSequences;
    }

    PressList getNumericSequence(const Presses& code)
    {
        PressList sequences {};
        Button position {Numeric::start};
        for (Press button : code)
        {
            // first move to the new button
            Button next {Numeric::keypadPos.at(button)};
            if (next != position)
            {
                PressList arrows {getNumericalPresses(position, next)};
                // bit wasteful to create a copy of the vector and re-assign
                // but safer than mixing things up
                sequences = addNewArrows(sequences, arrows);
            }
            position = next;
            // now press the button
            for (Presses& seq : sequences)
                seq.push_back('A');
        }
        return sequences;
    }

    PressList getDirectionalSequence(const Presses& prevSeq)
    {
        PressList sequences {};
        Button position {Directional::start};
        for (Press button : prevSeq)
        {
            // first move to the new button
            Button next {Directional::keypadPos.at(button)};
            if (next != position)
            {
                PressList arrows {getDirectionalPresses(position, next)};
                sequences = addNewArrows(sequences, arrows);
            }
            position = next;
            // now press the button
            for (Presses& seq : sequences)
                seq.push_back('A');
        }
        return sequences;
    }

    CodeInt shortestSequenceSize(const PressList& sequences)
    {
        size_t min {0};
        for (const Presses& seq : sequences)
        {
            if (min == 0 || seq.size() < min)
                min = seq.size();
        }
        // bit of a cheat here. Do properly if this was for something bigger
        return static_cast<long long>(min);
    }

    CodeInt getButtonPresses(const Presses& code)
    {
        // start with numeric keypad
        PressList numericSequence {getNumericSequence(code)};
        // move onto first directional keypad
        PressList directionalSeq1 {};
        for (const Presses& numSeq : numericSequence)
        {
            PressList newDir1 {getDirectionalSequence(numSeq)};
            directionalSeq1.insert(directionalSeq1.end(), newDir1.begin(), newDir1.end());
        }
        // move onto second directional keypad
        PressList directionalSeq2 {};
        for (const Presses& dirSeq : directionalSeq1)
        {
            PressList newDir2 {getDirectionalSequence(dirSeq)};
            directionalSeq2.insert(directionalSeq2.end(), newDir2.begin(), newDir2.end());
        }
        // find size of shortest final sequence
        return shortestSequenceSize(directionalSeq2);
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
            CodeInt buttonPresses {getButtonPresses(code)};
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
