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
#include <ranges>
#include <set>

/*
can't re-use part 1 solution as it took too long
seems like this needs solving through patterns/trends

can observe a few things
need to press 'A' after each button, so next has at least that added
then we need to account for the directions:
    ^ -> < -> ...
    > -> v -> ...
    v -> v< -> ...
    < -> v<< -> ...

we also know the number of "unit" moves needed for each direction
    ^ : 1
    > : 1
    v : 2
    < : 3
these are fixed. It's not possible to do less for any (from 'A')

double movement (e.g. "v<<") seems to be shorter than mixed (e.g. "<v<")
this is because the second in the double doesn't require moving

what we need is the distance between each tile, i.e.
    ^> : 2 (v>)
    <v : 1 (>)
    <A : 3 (>>^)

can try this as a way to find the shortest sequence
    ex1
    start : <vA
    size + 3 : to press A for each button
    A -> < : +3
    < -> v : +1
    v -> A : +2
    total : 9
    e.g. v<<A>A>^A

    ex2
    start : >^A
    size + 3
    A -> > : 1
    > -> ^ : 2
    ^ -> A : 1
    total : 7
    e.g. vA<^A>A

a question is about whether choice of direction matters
e.g. does the choice between ^<A and <^A matter in the long term?
    ^ : <A
    < : v<<A
    < -> A : >>^A
    ^ -> a : >A
    ^<A : <Av<A>>^A (9 = 3 + 1 + 2 + 3)
    <^A : v<<A>^A>A (9 = 3 + 3 + 2 + 1)

    the next iteration becomes either
    <Av<A>>^A : 9 + 3 + 3 + 2 + 1 + 3 + 1 + 0 + 2 + 1 = 25
        e.g. v<<A>>^Av<A<A>>^AvAA<^A>A
    v<<A>^A>A : 9 + 2 + 1 + 0 + 3 + 1 + 2 + 1 + 1 + 1 = 21
        e.g. v<A<AA>>^AvA<^A>AvA^A

so the choice does matter
the question then is: how do we know which sequence is shortest?
we could use the equation above to get the shortest size of each sequence
we could then keep only the shortest sequences
we would need to show that longer sequences could never "become shorter"
i.e. that in subsequent steps, a longer sequence translates to fewer moves
we can first note that we don't need to look at the whole sequence
each section is confined to ending at 'A', which resets the position
note above that ^<A and <^A both lead to sequences with the same moves
i.e. 2 * <, 2 * >, 1 * v, 1 * ^, 3 * A
while the order is different, they are made up of sub-sequences e.g. v<A
we then have a sub-problem of which order produces a shorter sequence

things to change:
    [change string vectors to int vector vectors] (leave this for now)
    add function to calculate minimum next sequence size
    use this function to only keep the shortest sequences
    use this in setup to find ordering giving shortest sequences
    apply ordering in puzzle to find shortest sequence directly
    i.e. we only have one sequence so we're not collecting 10^10 sequences

getting wrong answers for previous puzzle, with complexity too small
need to account for not going over blank spaces
bigger problem is it still takes too long to run

can we use double memoisation - like for day 11b?
if sequences follow a repeating pattern, then we might be able to
    ^ : [<], [v<<], [v<, v<<, v<<], ...
    > : [v], [v<], [v<, v<<], ...
    v : [v<], [v<], ...
    < : [v<<], [v<, v<<, v<<], ...
except it's not as simple as that. We need to take sequences
    ^> : [<, v>], [v<<, v<, v], [[v<, <, .], [v<, <], [v<]]
    <^^ : [v<<, >^, .], [[v<, <, .], [v, <^]]
that could work, tho also need to remember returning to 'A'
so first layer of memoisation to get to some breakpoint / pattern
and second layer of memoisation uses that to output size directly
that will avoid having to build the full sequence

the two layers of memoisation are speeding up things hugely
only issue now is getting the wrong answer
need to test getting solutions with the old approach vs this new one

found an error in finding shortest sequences
was not clearing vector when getting shorter sequences

now spotted that memo has some instances of going over empty space
e.g. <^A : v<<A, ^>A, >A
not sure how to handle that elegantly so will just hard-code the correction
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
    using UniquePress = std::set<Press>;
    using Movement = std::pair<Move, int>;
    using Button = char;
    using ButtonMap = std::map<std::pair<Button, Button>, PressList>;
    using Index = size_t;
    using Arrow = char;
    using Arrows = std::string;

    using Buttons = std::vector<Button>;
    using ButtonPair = std::pair<Button, Button>;
    using ButtonDistanceMap = std::map<ButtonPair, int>;
    using ShortestSequenceMap = std::map<Presses, Presses>;
    using PressLists = std::vector<PressList>;
    using PressCount = std::map<Button, int>;
    using UniquePresses = std::set<Presses>;

    using Memo = std::map<Presses, PressList>;
    using MemoSize = std::map<Presses, long long>;
    using FixedList = std::vector<Index>;
    using SequenceCount = std::map<Presses, long long>;

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

    ButtonDistanceMap getDirectionalButtonDistances()
    {
        ButtonDistanceMap distances {};
        auto buttonKeyView {std::views::keys(Directional::keypadPos)};
        Buttons buttonKeys {buttonKeyView.begin(), buttonKeyView.end()};
        for (Button key1 : buttonKeys)
        {
            for (Button key2 : buttonKeys)
            {
                ButtonLoc button1 {Directional::keypadPos.at(key1)};
                ButtonLoc button2 {Directional::keypadPos.at(key2)};
                distances[{key1, key2}] = std::abs(static_cast<int>(button1.first) - static_cast<int>(button2.first)) + std::abs(static_cast<int>(button1.second) - static_cast<int>(button2.second));
            }
        }
        return distances;
    }

    CodeInt getNextSequenceSize(const Presses& presses, const ButtonDistanceMap& buttonDistances)
    {
        // we add one 'A' for each button to press
        CodeInt total {static_cast<CodeInt>(presses.size())};
        for (size_t i{0}; i < presses.size(); ++i)
        {
            // the first button implicitly has a previous 'A'
            if (i == 0)
            {
                total += buttonDistances.at({'A', presses.at(i)});
            }
            // labelling as i > 0 to be explicit that i - 1 index is ok
            else if (i > 0)
            {
                total += buttonDistances.at({presses.at(i - 1), presses.at(i)});
            }
            else
                throw std::out_of_range("Error: got an unexpected button press, or button pair, with no known distance.\n");
        }
        return total;
    }

    void addSequence(Presses& presses, const Movement& movement)
    {
        for (int n{0}; n < movement.second; ++n)
            presses.push_back(getArrow(movement.first));
    }

    PressList getNumericalPresses(ButtonLoc start, ButtonLoc end, FixedList& fixed, Index index)
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

        // traverse each sequence to make sure it avoids the empty space
        PressList finalSequences {};
        for (const Presses& seq : sequences)
        {
            ButtonLoc position {start};
            Move previousMove {getDirection(seq.at(0))};
            bool wentOverEmptySpace {false};
            int moveChangeCounter {0};
            for (Press s : seq)
            {
                Move dir {getDirection(s)};
                if (dir != previousMove)
                    ++moveChangeCounter;
                position.first += dir.first;
                position.second += dir.second;
                previousMove = dir;
                // check if we've moved over the empty space
                if (Numeric::keypad.at(position.second).at(position.first) == ' ')
                {
                    wentOverEmptySpace = true;
                    fixed.push_back(index);
                }
            }
            // avoid changing direction twice or more
            if (!wentOverEmptySpace && moveChangeCounter <= 1)
            {
                finalSequences.push_back(seq);
            }
        }

        return finalSequences;
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
        // and if changing direction twice
        for (auto it{sequences.begin()}; it != sequences.end(); )
        {
            bool wentOverEmptySpace {false};
            int changeDirections {0};
            Press previousDir {it->at(0)};
            ButtonLoc current {start};
            for (Press dir : *it)
            {
                Move move {getDirection(dir)};
                current.first += move.first;
                current.second += move.second;
                if (Directional::keypad.at(current.second).at(current.first) == ' ')
                    wentOverEmptySpace = true;
                
                if (dir != previousDir)
                    ++changeDirections;
                previousDir = dir;
            }

            // drop the sequence if it goes over the empty space
            if (wentOverEmptySpace || changeDirections > 1)
                it = sequences.erase(it);
            else
                ++it;
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

    PressList getNumericSequence(const Presses& code, ButtonMap& numCodeMap, FixedList& fixed)
    {
        PressList sequences {};
        ButtonLoc position {Numeric::start};
        Button previous {'A'}; // we start on A
        Index index {0};
        for (Press button : code)
        {
            PressList arrows;
            ButtonLoc next {Numeric::keypadPos.at(button)};

            // if we've already mapped this button, re-use the answer
            if (numCodeMap.find({previous, button}) != numCodeMap.end())
                arrows = numCodeMap.at({previous, button});
            else
            {
                arrows = getNumericalPresses(position, next, fixed, index);
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
            ++index;
        }
        return sequences;
    }

    PressList getDirectionalSequence(const Presses& prevSeq, const ButtonDistanceMap& buttonDistances)
    {
        PressList sequences {};
        ButtonLoc position {Directional::start};
        for (Press button : prevSeq)
        {
            // first move to the new button
            ButtonLoc next {Directional::keypadPos.at(button)};
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

        // drop any sequences that are too long
        // we can get the target size of the next sequence
        int targetSz {static_cast<int>(getNextSequenceSize(prevSeq, buttonDistances))};
        for (auto it{sequences.begin()}; it != sequences.end();)
        {
            if (it->size() > targetSz)
                it = sequences.erase(it);
            else
                ++it;
        }

        return sequences;
    }

    PressList splitIntoSequences(const Presses& sequence)
    {
        PressList split {};
        Index current {0};
        Presses word {sequence};
        while (current < sequence.size())
        {
            Index nextA {word.find('A')};
            Presses nextSplit {word.substr(0, nextA)};
            split.push_back(nextSplit);
            current += nextA + 1;
            word = sequence.substr(current);
        }
        return split;
    }

    PressList getDirectionalShortSequences(const PressList& disallowedDirectionalSeq)
    {
        PressList sequences {};
        Buttons directions {'^', '>', 'v', '<'};
        // this is to create an object that can be re-used more easily
        PressCount allButtons {};
        // track which sequences we've tried already
        UniquePresses tried {};
        for (Button b : directions)
            allButtons[b] = 0;
        // this is a rubbish way to collect combinations
        for (size_t sz{1}; sz <= 3; ++sz)
        {
            for (Button b1 : directions)
            {
                for (Button b2 : directions)
                {
                    for (Button b3 : directions)
                    {
                        Presses seq {""};
                        seq.push_back(b1);
                        if (sz > 1)
                            seq.push_back(b2);
                        if (sz > 2)
                            seq.push_back(b3);
                        if (tried.find(seq) != tried.end())
                            continue;
                        tried.insert(seq);
                        
                        PressCount pressCounts {allButtons};
                        for (Button b : seq)
                            pressCounts[b]++;
                        
                        // don't collect where vertical >1
                        if (pressCounts.at('^') > 1 || pressCounts.at('v') > 1)
                            continue;
                        // don't collect where horizontal >2
                        if (pressCounts.at('>') > 2 || pressCounts.at('<') > 2)
                            continue;
                        // drop sequences going over directional blank
                        if (std::find(disallowedDirectionalSeq.begin(), disallowedDirectionalSeq.end(), seq) != disallowedDirectionalSeq.end())
                            continue;
                        // remove mixed vertical or horizontal
                        if ((pressCounts.at('^') > 0 && pressCounts.at('v') > 0) || (pressCounts.at('>') > 0 && pressCounts.at('<') > 0))
                            continue;
                        
                        sequences.push_back(seq);
                    }
                }
            }
        }
        return sequences;
    }

    // only for directional keypad sequences
    void getShortestSequences(ShortestSequenceMap& shorts, const PressList& sequences, const ButtonDistanceMap& buttonDistances, const PressList& disallowedSeq)
    {
        for (const Presses& s : sequences)
        {
            if (shorts.find(s) != shorts.end())
                continue;
            
            // to get permutations, need to sort string lexicographically
            Presses sorted {s};
            std::sort(sorted.begin(), sorted.end());
            PressList relatedSeq {};
            do
            {
                relatedSeq.push_back(sorted);
            } while (std::next_permutation(sorted.begin(), sorted.end()));
            
            PressList validSeq {};
            for (const Presses& seq : relatedSeq)
            {
                // skip any non-valid sequences
                if (std::find(disallowedSeq.begin(), disallowedSeq.end(), seq) != disallowedSeq.end())
                    continue;
                // skip any sequences that alternate
                int changeDirection {0};
                Press previousDir {seq.at(0)};
                for (size_t i{1}; i < seq.size(); ++i)
                {
                    if (seq.at(i) != previousDir)
                        ++changeDirection;
                    previousDir = seq.at(i);
                }
                if (changeDirection > 1)
                    continue;

                validSeq.push_back(seq);
            }

            Presses shortest {};
            PressLists current {};
            for (const Presses& start : validSeq)
            {
                Presses withA {start};
                withA.append("A");
                current.push_back({withA});
            }
            bool onlyOneShortest {false};
            // let's handle situation where there's only one valid sequence
            if (current.size() == 1)
                shortest = validSeq.at(0);
            while (!onlyOneShortest && current.size() > 1)
            {
                PressLists next {};
                int shortCount {0};
                Index shortSize {0};
                for (size_t i{0}; i < current.size(); ++i)
                {
                    PressList innerList {};
                    // let's only keep the shortest sequence
                    Index shor {0};
                    for (const Presses& currSeq : current.at(i))
                    {
                        PressList newSeq {getDirectionalSequence(currSeq, buttonDistances)};
                        PressList refinedList {};
                        for (const Presses& ns : newSeq)
                        {
                            if (shor == 0 || ns.size() < shor)
                            {
                                shor = ns.size();
                                refinedList.clear();
                                refinedList.push_back(ns);
                            }
                            else if (ns.size() == shor)
                            {
                                refinedList.push_back(ns);
                            }
                        }
                        if (innerList.size() > 0 && shor < innerList.at(0).size())
                            innerList.clear();
                        innerList.insert(innerList.end(), refinedList.begin(), refinedList.end());
                    }
                    next.push_back(innerList);
                    // collect info on the shortest sequence
                    if ((shortSize == 0 || innerList.at(0).size() < shortSize) && std::find(disallowedSeq.begin(), disallowedSeq.end(), validSeq.at(i)) == disallowedSeq.end())
                    {
                        shortSize = innerList.at(0).size();
                        shortCount = 0;
                        shortest = validSeq.at(i);
                    }
                    else if (innerList.at(0).size() == shortSize && std::find(disallowedSeq.begin(), disallowedSeq.end(), validSeq.at(i)) == disallowedSeq.end())
                    {
                        shortCount += 1;
                    }
                }
                current = next;

                if (shortCount == 0)
                    onlyOneShortest = true;
            }

            for (const Presses& seq : relatedSeq)
            {
                shorts[seq] = shortest;
            }
        }
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

    CodeInt shortestSequenceSize(const PressList& sequences)
    {
        Index min {0};
        for (const Presses& seq : sequences)
        {
            if (min == 0 || seq.size() < min)
            {
                min = seq.size();
            }
        }
        return static_cast<CodeInt>(min);
    }

    void setupMemo(Memo& memo, const ShortestSequenceMap& shortestSeq, const ButtonDistanceMap& buttonDistances)
    {
        // we only want to memoise the shortest sequences
        auto shortSeqValueView {std::views::values(shortestSeq)};
        PressList shortSeqValues {shortSeqValueView.begin(), shortSeqValueView.end()};
        // we'll collect unique and add 'A' to the end of sequences
        UniquePresses uniqueShortest {};
        for (const Presses& seq : shortSeqValues)
        {
            // don't bother if the sequence is already in memo
            if (memo.find(seq) != memo.end())
                continue;
            Presses addStr {seq};
            addStr.append("A");
            uniqueShortest.insert(addStr);
        }

        // we'll generate the next sequence for each and add to memo
        for (const Presses& seq : uniqueShortest)
        {
            if (memo.find(seq) != memo.end())
                continue;
            PressList nextSeq {getDirectionalSequence(seq, buttonDistances)};
            PressList nextShortestSeq {splitIntoSequences(nextSeq.at(0))};
            for (Presses& split : nextShortestSeq)
            {
                if (split != "")
                    split = shortestSeq.at(split);
                // need to add "A" to each sub-sequence
                split.append("A");
            }

            // now we can add to the memo
            memo[seq] = nextShortestSeq;
        }
    }

    MemoSize setupMemoSize(const Memo& memo)
    {
        MemoSize memoSize {};
        auto memoKeyView {std::views::keys(memo)};
        PressList memoKeys {memoKeyView.begin(), memoKeyView.end()};
        for (const Presses& key : memoKeys)
        {
            Index totalSz {0};
            for (const Presses& seq : memo.at(key))
                totalSz += seq.size();
            memoSize[key] = static_cast<long long>(totalSz);
        }
        return memoSize;
    }

    CodeInt combineAndGetSequenceSize(const PressList& seqSplit)
    {
        Presses sequence {};
        for (const Presses& seq : seqSplit)
            sequence.append(seq);
        return static_cast<CodeInt>(sequence.size());
    }

    bool doesNotPassOverEmptySpace(Button current, const Presses& seq)
    {
        ButtonLoc currLoc {Directional::keypadPos.at(current)};
        for (Press dir : seq)
        {
            Move move {getDirection(dir)};
            currLoc.first += move.first;
            currLoc.second += move.second;
            if (Directional::keypad.at(currLoc.second).at(currLoc.first) == ' ')
                return false;
        }
        return true;
    }

    template <int extraDirectionalRobots>
    CodeInt getButtonPresses(const Presses& code, ButtonMap& numCodeMap, const ButtonDistanceMap& buttonDistances, ShortestSequenceMap& shortestSeq, const Memo& memo, const MemoSize& memoSize)
    {
        // start with numeric keypad
        // we need a list of sequences not to change later
        FixedList fixed {};
        PressList numericSequence {getNumericSequence(code, numCodeMap, fixed)};

        // collect numeric splits
        PressList disallowedNumericSeq { "<<^^^", "<<vvv", ">>^^^", "^^^>>", "vvv>>" };
        PressList numericSplits {};
        for (const Presses& num : numericSequence)
        {
            PressList numSplits {splitIntoSequences(num)};
            numericSplits.insert(numericSplits.end(), numSplits.begin(), numSplits.end());
        }
        getShortestSequences(shortestSeq, numericSplits, buttonDistances, disallowedNumericSeq);

        // move onto directional keypads
        // if we prepare the initial numeric sequence, the rest should work
        PressList finalSeq {splitIntoSequences(numericSequence.at(0))};
        Index index {0};
        for (Presses& seq : finalSeq)
        {
            if (seq != "" && std::find(fixed.begin(), fixed.end(), index) == fixed.end())
                seq = shortestSeq.at(seq);
            seq.append("A");
            ++index;
        }

        if (extraDirectionalRobots == 0)
            return combineAndGetSequenceSize(finalSeq);

        PressList next {};
        for (const Presses& seq : finalSeq)
        {
            if (memo.find(seq) != memo.end())
            {
                next.insert(next.end(), memo.at(seq).begin(), memo.at(seq).end());
            }
            // this will only happen for initial numeric sequences
            // where an alternative sequence goes over the empty space
            else
            {
                PressList nextSeq {getDirectionalSequence(seq, buttonDistances)};
                PressList nextShortestSeq {splitIntoSequences(nextSeq.at(0))};
                // track which buttons we're moving between
                Button current {'A'};
                Index nextIndex {0};
                for (Presses& split : nextShortestSeq)
                {
                    if (split != "" && doesNotPassOverEmptySpace(current, shortestSeq.at(split)))
                        split = shortestSeq.at(split);
                    split.append("A");

                    current = seq.at(nextIndex);
                    ++nextIndex;
                }

                next.insert(next.end(), nextShortestSeq.begin(), nextShortestSeq.end());
            }
        }
        finalSeq = next;

        if (extraDirectionalRobots == 1)
            return combineAndGetSequenceSize(finalSeq);

        // we need to use the second layer of memoisation for the rest
        SequenceCount currentCount {};
        // need to set up the map
        // but this also counts as the second loop
        for (const Presses& seq : finalSeq)
        {
            currentCount[seq]++;
        }
        for (int loop{2}; loop < extraDirectionalRobots; ++loop)
        {
            SequenceCount nextCount {};
            for (const auto& [key, val] : currentCount)
            {
                for (const Presses& seq : memo.at(key))
                {
                    nextCount[seq] += val;
                }
            }
            currentCount = nextCount;
        }

        CodeInt finalSize {0};
        for (const auto& [key, val] : currentCount)
        {
            finalSize += static_cast<CodeInt>(val * memoSize.at(key));
        }
        return finalSize;
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
        
        // prepare this so we have distances between directional buttons
        ButtonDistanceMap buttonDistances {getDirectionalButtonDistances()};
        
        // set up order of directions giving shortest sequences
        PressList disallowedDirectionalSeq { "<<^", "<^<", "^<<", "<<v", ">>v", ">v>", "v>>", "^>>" };
        PressList directionalShorts {getDirectionalShortSequences(disallowedDirectionalSeq)};
        ShortestSequenceMap shortestSeq {};
        getShortestSequences(shortestSeq, directionalShorts, buttonDistances, disallowedDirectionalSeq);

        // set up first layer of memoisation
        Memo memo {};
        memo["A"] = {"A"};
        // rubbish hard-coding of some sequences
        // add this for the instance where we move from ^ to <
        memo["v<A"] = {"<vA", "<A", ">>^A"};
        // add this for the reverse instance where we move from < to ^
        memo[">^A"] = {"vA", "<^A", ">A"};
        setupMemo(memo, shortestSeq, buttonDistances);
        memo["<^A"] = {"v<<A", ">^A", ">A"};

        // set up second layer of memoisation
        // this is the number of button presses associated with a sequence
        MemoSize memoSize {setupMemoSize(memo)};

        for (const Presses& code : sequences)
        {
            complexity += getButtonPresses<extraDirectionalRobots>(code, numCodeMap, buttonDistances, shortestSeq, memo, memoSize) * getNumericPartOfCode(code);
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
