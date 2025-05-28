#ifndef AOC21A_H
#define AOC21A_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <stdexcept>

/*
CONSTRUCT combination of each weapon/item that can be used
ITERATE over each combination
IF combination wins the fight (for the player)
    IF cost for combination is lower than the current lowest
        THEN store this cost
    ELSE move to the next combination
ELSE move to the next combination
RETURN the lowest cost
*/

namespace aoc21a
{
    using GoldInt = int;

    template <std::size_t N>
    using Input = std::array<std::string_view, N>;

    using Stat = int;
    using Health = Stat;
    using Damage = Stat;
    using Defence = Stat;
    constexpr int PLAYER_HEALTH {100};
    // 0: health, 1: damage, 2: defence (armour)
    using Character = std::array<Stat, 3>;

    using Item = char;
    using Cost = Stat;
    // 0: cost, 1: damage, 2: defence (armour)
    using Weapon = std::array<Stat, 3>;
    using Weapons = std::vector<Weapon>;
    // 0: cost, 1: damage, 2: defence (armour)
    using Armour = std::array<Stat, 3>;
    using Armours = std::vector<Armour>;
    // 0: cost, 1: damage, 2: defence (armour)
    using Ring = std::array<Stat, 3>;
    using Rings = std::vector<Ring>;

    template <std::size_t N>
    Character readBoss(const Input<N>& boss)
    {
        Character bossStats {};
        size_t index {0};
        for (std::string_view row : boss)
        {
            std::string line {std::string(row)};
            size_t colon {line.find(':')};
            Stat individualStat {std::stoi(line.substr(colon + 2))};

            bossStats[index] = individualStat;
            ++index;
        }
        return bossStats;
    }

    template <std::size_t N>
    void getShopItems(const Input<N>& shop, Weapons& weapons, Armours& armours, Rings& rings)
    {
        Item activeItem {};
        for (std::string_view row : shop)
        {
            if (row.empty())
                continue;

            std::string line {std::string(row)};
            if (line.find(':') != std::string::npos)
            {
                activeItem = line.at(0);
                continue;
            }

            size_t firstSpace {line.find(' ')};
            size_t secondSpace {line.find(' ', firstSpace + 1)};

            size_t startCost {line.find_first_not_of(' ', secondSpace + 1)};
            size_t afterCost {line.find(' ', startCost)};
            Cost cost {std::stoi(line.substr(startCost, afterCost - startCost))};

            size_t startDamage {line.find_first_not_of(' ', afterCost)};
            size_t afterDamage {line.find(' ', startDamage)};
            Damage damage {std::stoi(line.substr(startDamage, afterDamage - startDamage))};

            size_t startDefence {line.find_first_not_of(' ', afterDamage)};
            Defence defence {std::stoi(line.substr(startDefence))};

            if (activeItem == 'W')
            {
                weapons.push_back({cost, damage, defence});
            }
            else if (activeItem == 'A')
            {
                armours.push_back({cost, damage, defence});
            }
            else if (activeItem == 'R')
            {
                rings.push_back({cost, damage, defence});
            }
            else
                throw std::invalid_argument("Didn't have an active item in the shop.\n");
        }
    }

    bool doesPlayerWin(const Character& boss, const Character& player)
    {
        Health bossHealth {boss.at(0)};
        Health playerHealth {player.at(0)};

        while (bossHealth > 0 && playerHealth > 0)
        {
            // attack the boss!
            bossHealth -= (player.at(1) - boss.at(2) < 1) ? 1 : player.at(1) - boss.at(2);
            if (bossHealth <= 0)
                return true;

            // boss attack!
            playerHealth -= (boss.at(1) - player.at(2) < 1) ? 1 : boss.at(1) - player.at(2);
            if (playerHealth <= 0)
                return false;
        }
        throw std::out_of_range("Did not get a clear winner.\n");
    }

    GoldInt findCheapestWin(const Character& boss, const Weapons& weapons, const Armours& armours, const Rings& rings)
    {
        GoldInt cheapest {};
        // need to kit out the player
        for (const Weapon& w : weapons)
        {
            for (int a{-1}; a < static_cast<int>(armours.size()); ++a)
            {
                for (int r1{-1}; r1 < static_cast<int>(rings.size()); ++r1)
                {
                    for (int r2{r1 + 1}; r2 < static_cast<int>(rings.size()); ++r2)
                    {
                        Cost totalCost {w.at(0) + rings.at(r2).at(0)};
                        Damage totalDamage {w.at(1) + rings.at(r2).at(1)};
                        Defence totalDefence {w.at(2) + rings.at(r2).at(2)};
                        if (a >= 0)
                        {
                            totalCost += armours.at(a).at(0);
                            totalDamage += armours.at(a).at(1);
                            totalDefence += armours.at(a).at(2);
                        }
                        if (r1 >= 0)
                        {
                            totalCost += rings.at(r1).at(0);
                            totalDamage += rings.at(r1).at(1);
                            totalDefence += rings.at(r1).at(2);
                        }

                        Character player {PLAYER_HEALTH, totalDamage, totalDefence};

                        if (doesPlayerWin(boss, player) && (cheapest == 0 || totalCost < cheapest))
                        {
                            cheapest = totalCost;
                        }
                    }
                }
            }
        }
        return cheapest;
    }

    template <std::size_t L, std::size_t M>
    GoldInt getLeastGoldToWin(const Input<L>& boss, const Input<M>& shop)
    {
        Character bossStats {readBoss<L>(boss)};

        Weapons weapons {};
        Armours armours {};
        Rings rings {};
        getShopItems<M>(shop, weapons, armours, rings);

        return findCheapestWin(bossStats, weapons, armours, rings);
    }
}

#endif // AOC21A_H
