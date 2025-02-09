#ifndef AOC21B_H
#define AOC21B_H

#include <string_view>
#include <array>
#include <vector>
#include <string>
#include "../../shared/Direction.h"
#include "../../shared/Array.h"
#include <map>
#include <stdexcept>
#include <algorithm>

#include <iostream>
#include "../../shared/Timer.h"

/*
can't re-use part 1 solution as it took too long
is there some pattern in how the complexity increases?
or could I optimise my part 1 solution to be significantly faster?
worth trying optimising first and see the result
not really seeing where to

the set of moves between buttons will always be the same
so we can use memoisation to reduce
using buttons only really works for the first keypad
need to use longer (full?) sequences for the directional part

need to set up partial memoisation since directional codes get longer

the biggest slowdown seems to be the addNewArrows() function
this copies a vector of strings then re-assigns this
perhaps instead of collecting the string vector, we can use the map?
e.g. add string vector to map and track what the current key (string) is

optimising doesn't seem to be the way to go - it'll take too long anyway
need to take a different approach
is there a way of finding a pattern in the size of the shortest sequence?
if we can find the shortest sequence to start, we don't need combinations
is there a way to predict/understand the shortest sequence?
if we have <<^, <^<, ^<< - can we tell what will be shortest in n keypads?
<<^ -> v<<AA>^A   -> v<A<AA>>^AAvA<^A>A
<^< -> v<<A>^Av<A -> v<A<AA>>^AvA<^A>Av<A<A>>^A
^<< -> <Av<AA     -> v<<A>>^Av<A<A>>^AA
it appears that consecutive arrows of same type is better?

<^ -> v<<A>^A -> v<A<AA>>^AvA<^A>A
^< -> <Av<A   -> v<<A>>^Av<A<A>>^A

is this a case where it could be useful to set up things first
i.e. run all iterations of 1-3 arrows (e.g. >, <^, vv>) at the start
and then use that to essentially shortcut the answer
i.e. we can find the shortest sequence after n keypads/iterations
we can guarantee getting to any arrow (on directional keybad) in 3 arrows
make sure to add 'A' to the end, so we can use this in the subsequent code
the longest sequence for the numeric keypad is 5 arrows

rather than setting up every permutation, use numeric sequence to decide
saves from doing sequences that aren't used

still takes too long, look at patterns in shortest sequences and preceding
may be able to identify mini-sequences to exclude etc
*/

using ButtonLoc = std::pair<size_t, size_t>;
using KeypadNum = Array::Array2d<char, 4, 3>;
using KeypadDir = Array::Array2d<char, 2, 3>;
using KeypadMap = std::map<char, ButtonLoc>;

namespace Numeric
{
    constexpr ButtonLoc start {2, 3}; // starts on 'A'
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
    constexpr ButtonLoc start {2, 0}; // starts on 'A'
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

namespace aoc21b
{
    using CodeInt = long long;
    using Move = std::pair<int, int>;
    using Press = char;
    using Presses = std::string;
    using PressList = std::vector<Presses>;
    using Movement = std::pair<Move, int>;
    using Button = char;
    using ButtonMap = std::map<std::pair<Button, Button>, PressList>;
    using PressMap = std::map<std::pair<Press, Press>, PressList>;
    using FullPressMap = std::map<Presses, PressList>;
    using Index = size_t;
    using Arrow = char;
    using Arrows = std::string;
    using ArrowMap = std::map<Arrows, Presses>;
    using ArrowList = std::vector<Arrows>;

    constexpr Move getDirection(char c)
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

    constexpr Press getArrow(Move move)
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

    Movement getHorizontalMove(ButtonLoc start, ButtonLoc end)
    {
        if (end.first > start.first)
            return {Direction::east, end.first - start.first};
        else if (end.first < start.first)
            return {Direction::west, start.first - end.first};
        // no horizontal movement
        else
            return {{0, 0}, 0};
    }

    Movement getVerticalMove(ButtonLoc start, ButtonLoc end)
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

    PressList getNumericalPresses(ButtonLoc start, ButtonLoc end)
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

    PressList getDirectionalPresses(ButtonLoc start, ButtonLoc end)
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
        // doing this to try to save time - it may cause issues
        newSequences.reserve(sequences.size() * arrows.size());
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

    // trying this alternative function in case it's faster
    void addNewArrowsInPlace(const Presses& previous, const Presses& current, FullPressMap& dirSeqMap, const PressList& arrows)
    {
        if (dirSeqMap.find(current) != dirSeqMap.end())
            return;
        
        if (previous.empty())
        {
            dirSeqMap[current] = arrows;
            return;
        }

        PressList newSequences {};
        for (const Presses& seq : dirSeqMap.at(previous))
        {
            for (const Presses& newArrows : arrows)
            {
                Presses temp {seq};
                temp.append(newArrows);
                newSequences.push_back(std::move(temp));
            }
        }
        dirSeqMap[current] = newSequences;
        // std::cout << '\t' << dirSeqMap.at(previous).size() << " * " << arrows.size() << ' ' << dirSeqMap.at(current).size() << '\n';
    }

    PressList getNumericSequence(const Presses& code, ButtonMap& numCodeMap)
    {
        PressList sequences {};
        ButtonLoc position {Numeric::start};
        Button previous {'A'}; // we start on A
        for (Press button : code)
        {
            PressList arrows;
            ButtonLoc next {Numeric::keypadPos.at(button)};

            // if we've already mapped this button, re-use the answer
            if (numCodeMap.find({previous, button}) != numCodeMap.end())
                arrows = numCodeMap.at({previous, button});
            else
            {
                arrows = getNumericalPresses(position, next);
                // now press the button
                for (Presses& seq : arrows)
                    seq.push_back('A');
                // store these solutions so we can re-use them later
                numCodeMap[{previous, button}] = arrows;
            }
            // bit wasteful to create a copy of the vector and re-assign
            // but safer than mixing things up
            sequences = addNewArrows(sequences, arrows);
            
            position = next;
            previous = button;
        }
        return sequences;
    }

    PressList getDirectionalSequence(const Presses& prevSeq, PressMap& dirCodeMap, FullPressMap& dirSeqMap)
    {
        // if we already have run the full sequence, re-use the solution
        // this doesn't seem to activate
        if (dirSeqMap.find(prevSeq) != dirSeqMap.end())
            return dirSeqMap.at(prevSeq);

        Presses previousStr;
        Presses seqExtract {prevSeq};
        Index currIndex {0};
        while (currIndex < prevSeq.size())
        {
            PressList arrows {};
            bool isMatch {false};

            // first check if we have an 'A', or several
            if (seqExtract[0] == 'A')
            {
                Index firstNonA {seqExtract.find_first_not_of('A')};
                // if we have a sequence of 'A's, we can re-use that
                arrows = {seqExtract.substr(0, firstNonA)};
                currIndex += firstNonA;
                isMatch = true;
            }

            Index latest {seqExtract.size() - 1};
            while (!isMatch && seqExtract.rfind('A', latest) != std::string::npos)
            {
                // // skip if the next instruction is 'A'
                // if (seqExtract[0] == 'A')
                //     break;
                Index last {seqExtract.rfind('A', latest)};
                if (dirSeqMap.find(seqExtract.substr(0, last + 1)) != dirSeqMap.end())
                {
                    arrows = dirSeqMap.at(seqExtract.substr(0, last + 1));
                    isMatch = true;
                    currIndex += last + 1;
                    break;
                }
                else
                {
                    if (last == 0)
                        break;
                    latest = last - 1;
                }
            }

            // if the above didn't find a sequence to re-use
            // we'll instead go up to the first 'A' and get the sequence
            if (!isMatch)
            {
                Index firstA {seqExtract.find('A')};
                Presses strUptoA {seqExtract.substr(0, firstA + 1)};
                PressList internalSeq {};
                ButtonLoc position {Directional::start};
                Press previous {'A'}; // start on 'A'
                for (Press move : strUptoA)
                {
                    PressList internalArrow;
                    ButtonLoc next {Directional::keypadPos.at(move)};

                    // if we've already mapped this button, re-use the answer
                    if (dirCodeMap.find({previous, move}) != dirCodeMap.end())
                        internalArrow = dirCodeMap.at({previous, move});
                    else
                    {
                        internalArrow = getDirectionalPresses(position, next);
                        // now press the button
                        for (Presses& seq : internalArrow)
                            seq.push_back('A');
                        // store these solutions so we can re-use them later
                        dirCodeMap[{previous, move}] = internalArrow;
                    }

                    internalSeq = addNewArrows(internalSeq, internalArrow);

                    position = next;
                    previous = move;
                }
                // collect sequence so we can re-use it later
                arrows = internalSeq;
                // dirSeqMap[strUptoA] = arrows;
                currIndex += firstA + 1;
            }

            // sequences = addNewArrows(sequences, arrows);
            // // collect new sequences if we don't have them already
            // if (dirSeqMap.find(prevSeq.substr(0, currIndex)) == dirSeqMap.end())
            //     dirSeqMap[prevSeq.substr(0, currIndex)] = sequences;
            addNewArrowsInPlace(previousStr, prevSeq.substr(0, currIndex), dirSeqMap, arrows);
            previousStr = prevSeq.substr(0, currIndex);
            seqExtract = prevSeq.substr(currIndex);
        }
        // // collect sequence so we can re-use it later
        // dirSeqMap[prevSeq] = sequences;

        return dirSeqMap.at(prevSeq);
    }

    PressList splitIntoSequences(const PressList& sequences)
    {
        PressList split {};
        for (const Presses& seq : sequences)
        {
            Index current {0};
            Presses word {seq};
            while (current < seq.size())
            {
                Index nextA {word.find('A')};
                Presses nextSplit {word.substr(0, nextA + 1)};
                split.push_back(nextSplit);
                current += nextA + 1;
                word = seq.substr(current);
            }
        }
        return split;
    }

    Presses shortestSequence(const PressList& sequences)
    {
        Presses shortest;
        for (const Presses& seq : sequences)
        {
            if (shortest.size() == 0 || seq.size() < shortest.size())
                shortest = seq;
        }
        return shortest;
    }

    template <int extraDirectionalRobots>
    Presses getShortestArrowSequence(const Arrows& arrow, PressMap& dirCodeMap, FullPressMap& dirSeqMap)
    {
        PressList currentSeq {arrow};
        for (int r{0}; r < extraDirectionalRobots; ++r)
        {
            PressList newSeq {};
            for (const Presses& seq : currentSeq)
            {
                PressList newDir {getDirectionalSequence(seq, dirCodeMap, dirSeqMap)};
                newSeq.insert(newSeq.end(), newDir.begin(), newDir.end());
            }
            currentSeq = std::move(newSeq);
            // Presses shortest {shortestSequence(newSeq)};
            // currentSeq = {shortest};
        }
        return shortestSequence(currentSeq);
    }

    template <int extraDirectionalRobots>
    void trainArrows(const ArrowList& arrows, ArrowMap& shortestArrows, PressMap& dirCodeMap, FullPressMap& dirSeqMap)
    {
        // int counter {1};
        for (const Arrows& arw : arrows)
        {
            // std::cout << "\tat: " << counter++ << '\n';
            shortestArrows[arw] = getShortestArrowSequence<extraDirectionalRobots>(arw, dirCodeMap, dirSeqMap);
        }
    }

    CodeInt shortestSequenceSize(const PressList& sequences)
    {
        size_t min {0};
        Presses test;
        for (const Presses& seq : sequences)
        {
            if (min == 0 || seq.size() < min)
            {
                min = seq.size();
                test = seq;
            }
        }
        // bit of a cheat. Do properly if this was for something bigger
        std::cout << '\t' << test << '\n';
        return static_cast<long long>(min);
    }

    template <int extraDirectionalRobots>
    CodeInt getButtonPresses(const Presses& code, ButtonMap& numCodeMap, ArrowMap& shortestArrowSequences, PressMap& dirCodeMap, FullPressMap& dirSeqMap)
    {
        // start with numeric keypad
        PressList numericSequence {getNumericSequence(code, numCodeMap)};
        // std::cout << "shortest num " << shortestSequenceSize(numericSequence) << '\n';

        ArrowList arrowSequences {splitIntoSequences(numericSequence)};
        trainArrows<extraDirectionalRobots>(arrowSequences, shortestArrowSequences, dirCodeMap, dirSeqMap);

        // move onto directional keypads
        // use prepared shortestArrowSequences to jump to solution
        PressList finalSeq {};
        for (const Presses& numSeq : numericSequence)
        {
            Presses seqShort;
            Index currIndex {0};
            // std::cout << "seq: " << numSeq << '\n';
            while (currIndex < numSeq.size())
            {
                Index nextA {numSeq.find('A', currIndex)};
                // std::cout << "looking for " << numSeq.substr(currIndex, (nextA + 1) - currIndex) << ' ' << currIndex << '\n';
                // std::cout << std::boolalpha;
                // std::cout << "result: " << (shortestArrowSequences.find(numSeq.substr(currIndex, (nextA + 1) - currIndex)) != shortestArrowSequences.end()) << '\n';
                seqShort += shortestArrowSequences.at(numSeq.substr(currIndex, (nextA + 1) - currIndex));
                // std::cout << "test\n";
                currIndex = nextA + 1;
            }
            finalSeq.push_back(seqShort);
        }
        return shortestSequenceSize(finalSeq);
    }

    CodeInt getNumericPartOfCode(const Presses& code)
    {
        return std::stoll(code.substr(0, code.size() - 1));
    }

    template <int extraDirectionalRobots>
    CodeInt getCodeComplexity(const PressList& sequences)
    {
        CodeInt complexity {0};
        ButtonMap numCodeMap {};
        ArrowMap shortestArrowSequences {};
        PressMap dirCodeMap {};
        FullPressMap dirSeqMap {};
        for (const Presses& code : sequences)
        {
            std::cout << "for " << code << '\n';
            CodeInt old {complexity};
            complexity += getButtonPresses<extraDirectionalRobots>(code, numCodeMap, shortestArrowSequences, dirCodeMap, dirSeqMap) * getNumericPartOfCode(code);
            // std::cout << "after " << (complexity - old) / getNumericPartOfCode(code) << ' ' << getNumericPartOfCode(code) << ", " << (complexity - old) << ' ' << complexity << '\n';
        }
        return complexity;
    }

    template <std::size_t N, int extraDirectionalRobots>
    CodeInt parseAndGetCodeComplexity(const std::array<std::string_view, N>& lines)
    {
        PressList sequences {};
        for (std::string_view line : lines)
            sequences.push_back(std::string(line));
        return getCodeComplexity<extraDirectionalRobots>(sequences);
    }
}

#endif // AOC21B_H
