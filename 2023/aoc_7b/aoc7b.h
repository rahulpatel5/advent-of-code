#ifndef AOC7B_H
#define AOC7B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <ranges>
#include <stdexcept>
#include <cassert>

/*
FOR each hand:
    ASSIGN a score based on the rules
    [IDENTIFY highest count & score non-jack card and add jack count to it]

RANK each hand from low score (1) to highest
    [SET jack to new lowest score (0)]

RETURN sum product of rank and bid
*/

namespace aoc7b
{
    using WinningInt = long long;
    template <std::size_t L>
    using Lines = std::array<std::string_view, L>;

    using Card = char;
    using Hand = std::string;
    using Hands = std::vector<Hand>;
    using Bid = long long;
    using Bids = std::vector<Bid>;

    using CardDict = std::map<Card, int>;
    using Type = long long;
    using Types = std::vector<Type>;
    using Score = long long;
    using Scores = std::vector<Score>;
    using Rank = long long;
    using Ranks = std::vector<Rank>;
    using Index = size_t;
    using Order = std::vector<Index>;
    using RankDict = std::map<Rank, Order>;

    const int maxCardValue {13};

    template <std::size_t N>
    void readHandsAndBids(Hands& hands, Bids& bids, const Lines<N>& lines)
    {
        for (std::string_view line : lines)
        {
            std::string row { std::string(line) };
            Hand hand { row.substr(0, 5) };
            hands.push_back(hand);

            size_t space { row.find(' ') };
            std::string bidStr { row.substr(space + 1) };
            Bid bid { std::stoll(bidStr) };
            bids.push_back(bid);
        }
    }

    int getCardRanking(Card card)
    {
        if (card >= '2' && card <= '9')
        {
            return card - '1';
        }
        else if (card == 'J')
            return 0;
        else if (card == 'T')
            return 9;
        else if (card == 'J')
            return 10;
        else if (card == 'Q')
            return 11;
        else if (card == 'K')
            return 12;
        else if (card == 'A')
            return 13;
        else
            throw std::out_of_range("Got non-valid card with no associated value.");
    }

    Card getBestCardForJoker(const CardDict& dict)
    {
        Card best {};
        Rank maxCount {-1};
        Score highestScore {-1};
        for (auto [card, count] : dict)
        {
            if (card == 'J')
                continue;

            if (count > maxCount)
            {
                best = card;
                highestScore = getCardRanking(card);
                maxCount = count;
            }
            else if (count == maxCount && getCardRanking(card) > highestScore)
            {
                best = card;
                highestScore = getCardRanking(card);
            }
        }
        return best;
    }

    Type getHandType(const Hand& hand)
    {
        // collect number of each card
        CardDict cardDict {};
        for (Card c : hand)
        {
            cardDict[c]++;
        }

        if (cardDict.find('J') != cardDict.end())
        {
            Card bestCard { getBestCardForJoker(cardDict) };
            cardDict[bestCard] += cardDict.at('J');
            cardDict.erase('J');
        }

        // highest score for five of a kind
        if (cardDict.size() == 1)
            return 7;
        // one pair
        else if (cardDict.size() == 4)
            return 2;
        // lowest score for unique cards
        else if (cardDict.size() == 5)
            return 1;
        else
        {
            // get max number of cards of the same value
            int max { *std::ranges::max_element(std::views::values(cardDict)) };

            if (cardDict.size() == 2)
            {
                // four of a kind
                if (max == 4)
                    return 6;
                // full house
                else if (max == 3)
                    return 5;
                else
                    throw std::out_of_range("Getting unexpected type for 2 unique card hand.\n");
            }
            else if (cardDict.size() == 3)
            {
                // three of a kind
                if (max == 3)
                    return 4;
                // two pairs
                else if (max == 2)
                    return 3;
                else
                    throw std::out_of_range("Getting unexpected type for 3 unique card hand.\n");
            }
            else
                throw std::out_of_range("Getting unexpected hand type.\n");
        }
    }

    Score getScore(Hand hand)
    {
        Score score {};
        long long factor {1};
        // reverse card order because decreasing order of importance
        std::reverse(hand.begin(), hand.end());
        for (char c : hand)
        {
            score += getCardRanking(c) * factor;
            factor *= (maxCardValue + 1);
        }
        return score;
    }

    void getHandScores(Types& types, Scores& scores, const Hands& hands)
    {
        for (const Hand& hand : hands)
        {
            Type type { getHandType(hand) };
            types.push_back(type);

            Score score { getScore(hand) };
            scores.push_back(score);
        }
    }

    Ranks getHandRanks(const Types& types, const Scores& scores)
    {
        Ranks ranks {};
        Rank sentinelValue {-1};
        for (size_t i{0}; i < types.size(); ++i)
        {
            ranks.push_back(sentinelValue);
        }

        Rank rankValue {1};
        Order minPositions {};
        while (std::find(ranks.begin(), ranks.end(), sentinelValue) != ranks.end())
        {
            RankDict rankDict {};
            for (size_t k{0}; k < types.size(); ++k)
            {
                if (std::find(minPositions.begin(), minPositions.end(), k) != minPositions.end())
                    continue;
                
                rankDict[types.at(k)].push_back(k);
            }
            
            // ordering of maps means min key value is first
            Order minIndexes {rankDict.begin()->second};

            RankDict scoreDict {};
            for (Index index : minIndexes)
            {
                scoreDict[scores.at(index)].push_back(index);
            }
            // there should only be one value per key
            assert(scoreDict.size() == minIndexes.size() && "Map of scores does not match the number of elements with the min value.\n");

            for (const Order& o : scoreDict | std::views::values)
            {
                ranks[o.at(0)] = rankValue;
                ++rankValue;
            }

            for (Index index : minIndexes)
            {
                minPositions.push_back(index);
            }
        }

        return ranks;
    }

    WinningInt getWinnings(const Bids& bids, const Ranks& ranks)
    {
        WinningInt winnings {};

        for (size_t i{0}; i < bids.size(); ++i)
        {
            winnings += bids.at(i) * ranks.at(i);
        }

        return winnings;
    }

    template <std::size_t N>
    WinningInt getTotalWinnings(const Lines<N>& lines)
    {
        Hands hands {};
        Bids bids {};
        readHandsAndBids<N>(hands, bids, lines);

        Types handTypes {};
        Scores handScores {};
        getHandScores(handTypes, handScores, hands);
        Ranks ranks { getHandRanks(handTypes, handScores) };
        assert(bids.size() == ranks.size() && "Number of bids is not the same as the number of ranks.\n");

        return getWinnings(bids, ranks);
    }
}

#endif // AOC7B_H
