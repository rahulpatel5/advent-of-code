#ifndef AOC22B_H
#define AOC22B_H

#include <array>
#include <string_view>
#include <string>
#include <vector>
#include <cassert>
#include <stdexcept>

/*
repeat part 1 solution, but add player turn health reduction

change to prioritise certain spells in certain conditions
need to be sure when doing this, as will skip options

WHILE player or boss haven't run out of health
    IF player doesn't have enough mana to cast spell
        THEN player loses
        TRY a different branch of moves
    ELSE
        TRACK mana cost from player casting spell
    
    APPLY ongoing spell effects
    TRACK where spell effects end

    IF spell damage/effect causes boss to run out of health
        THEN player wins
        IF mana cost is less than current minimum
            STORE mana cost as new minimum
        TRY a different branch of moves
    ELSE
        APPLY spell or effect to player or boss
        BRANCH into different choices from this point
    
    APPLY ongoing spell effects
    TRACK if spell effects end
    [CHECK if boss has run out health]

    Boss attacks player
    IF player runs out of health
        THEN player loses
        TRY a different branch of moves
    ELSE
        REDUCE player health
        * CHOOSE next spell based on conditions
            prioritise having shield, unless almost won
            then prioritise using poison, as most cost effective
            then choose default path
        MOVE to next round

RETURN the minimum mana needed to win

* shows a change to the part 1 solution
*/

namespace aoc22b
{
    using Mana = int;
    template <std::size_t N>
    using Lines = std::array<std::string_view, N>;

    using Attr = int;
    using Stats = std::vector<Attr>;
    using Health = Attr;
    using Damage = Attr;
    using Armour = Attr;

    struct Boss {
        Health health;
        Damage attack;
    };
    struct Player {
        Health health;
        Mana mana;
    };

    using Spell = int;
    using SpellInfo = std::pair<Mana, Damage>;
    using SpellLength = int;
    using SpellCosts = std::array<Mana, 5>;

    constexpr Health PLAYER_HEALTH {50};
    constexpr Mana PLAYER_MANA {500};

    constexpr SpellInfo MAGIC_MISSILE {std::make_pair(53, 4)};
    constexpr SpellInfo DRAIN {std::make_pair(73, 2)};
    constexpr Mana SHIELD_COST {113};
    constexpr SpellLength SHIELD_MAX_TURNS {6};
    constexpr Armour SHIELD_EFFECT {7};
    constexpr Mana POISON_COST {173};
    constexpr SpellLength POISON_MAX_TURNS {6};
    constexpr Damage POISON_DAMAGE_PER_TURN {3};
    constexpr Mana RECHARGE_COST {229};
    constexpr SpellLength RECHARGE_MAX_TURNS {5};
    constexpr Mana RECHARGE_REGAIN_PER_TURN {101};
    constexpr size_t MAX_SPELL_CHOICES {5};
    // 0 = shield turns, 1 = poison turns, 2 = recharge turns
    using SpellTurns = std::array<SpellLength, 3>;

    template <std::size_t N>
    Boss getBossInfo(const Lines<N>& bossInfo)
    {
        Stats stats {};
        for (std::string_view row : bossInfo)
        {
            if (row.find(':') != std::string_view::npos)
            {
                std::string line {std::string(row)};
                size_t colon {line.find(':')};
                stats.push_back(std::stoi(line.substr(colon + 2)));
            }
        }
        assert(stats.size() == 2 && "Expected there to be two stats for the boss.\n");
        return {stats.at(0), stats.at(1)};
    }

    void applySpellEffectsAndReduceTurn(Player& player, Boss& boss, SpellTurns& spellTurns)
    {
        for (size_t s{0}; s < spellTurns.size(); ++s)
        {
            if (spellTurns.at(s) > 0)
            {
                if (s == 1)
                    boss.health -= POISON_DAMAGE_PER_TURN;
                else if (s == 2)
                    player.mana += RECHARGE_REGAIN_PER_TURN;

                --spellTurns[s];
            }
        }
    }

    Mana recursiveMinimumMana(Player player, Boss boss, const SpellCosts& spellCosts, size_t spellIndex, SpellTurns spellTurns, Mana manaSpent, Mana& minMana)
    {
        for (size_t i{spellIndex}; i < MAX_SPELL_CHOICES; ++i)
        {
            if (player.health <= 1 || boss.health <= 0 || (minMana != 0 && manaSpent >= minMana))
                break;
            
            if ((i > 1 && spellTurns.at(i - 2) > 1) || (player.mana + ((spellTurns.at(2) > 0) ? RECHARGE_REGAIN_PER_TURN : 0) < spellCosts.at(i)))
                continue;

            // only continue if there's any point
            if (player.health > 1 && boss.health > 0 && (minMana == 0 || manaSpent < minMana) && player.mana > 0)
                recursiveMinimumMana(player, boss, spellCosts, i + 1, spellTurns, manaSpent, minMana);
            
            // player turn
            // new health reduction effect
            player.health -= 1;
            assert(player.health > 0 && "Expected player to still be alive.\n");
            applySpellEffectsAndReduceTurn(player, boss, spellTurns);
            
            if (boss.health <= 0 && player.health > 0 && (minMana == 0 || manaSpent < minMana))
            {
                minMana = manaSpent;
            }
            
            manaSpent += spellCosts.at(i);
            player.mana -= spellCosts.at(i);
            if (i == 0)
                boss.health -= MAGIC_MISSILE.second;
            else if (i == 1)
            {
                boss.health -= DRAIN.second;
                player.health += DRAIN.second;
            }
            else if (i == 2)
                spellTurns[0] = SHIELD_MAX_TURNS;
            else if (i == 3)
                spellTurns[1] = POISON_MAX_TURNS;
            else if (i == 4)
                spellTurns[2] = RECHARGE_MAX_TURNS;
            else
                throw std::out_of_range("Got unexpected spell.\n");
            
            // boss turn
            bool isShieldActive {spellTurns.at(0) > 0};
            applySpellEffectsAndReduceTurn(player, boss, spellTurns);

            if (boss.health <= 0 && player.health > 0 && (minMana == 0 || manaSpent < minMana) && player.mana >= 0)
            {
                minMana = manaSpent;
            }
            
            Damage bossAttack {boss.attack - (isShieldActive ? SHIELD_EFFECT : 0)};
            player.health -= (bossAttack < 1) ? 1 : bossAttack;

            if (player.health > 1 && boss.health > 0 && (minMana == 0 || manaSpent < minMana))
            {
                // avoid damage to stay alive, unless almost won
                if (spellTurns.at(0) <= 1 && (boss.health > MAGIC_MISSILE.second * 2 + POISON_DAMAGE_PER_TURN * ((spellTurns.at(1) < 3) ? spellTurns.at(1) : 2)) && player.health > 3)
                    recursiveMinimumMana(player, boss, spellCosts, 2, spellTurns, manaSpent, minMana);
                // poison is most cost effective spell
                else if (spellTurns.at(1) <= 1 && boss.health > POISON_DAMAGE_PER_TURN * POISON_MAX_TURNS)
                    recursiveMinimumMana(player, boss, spellCosts, 3, spellTurns, manaSpent, minMana);
                else
                    recursiveMinimumMana(player, boss, spellCosts, 0, spellTurns, manaSpent, minMana);
            }
        }
        return minMana;
    }

    template <std::size_t L>
    Mana getLeastManaToWin(const Lines<L>& bossInfo)
    {
        Player player {PLAYER_HEALTH, PLAYER_MANA};
        Boss boss {getBossInfo<L>(bossInfo)};

        SpellTurns spellTurns {};
        Mana manaSpent {0};
        Mana minManaToWin {};
        SpellCosts spellCosts {
            MAGIC_MISSILE.first,
            DRAIN.first,
            SHIELD_COST,
            POISON_COST,
            RECHARGE_COST
        };

        // 0 = magic missile, 1 = drain, 2 = shield
        // 3 = poison, 4 = recharge
        size_t spellIndex {0};
        return recursiveMinimumMana(player, boss, spellCosts, spellIndex, spellTurns, manaSpent, minManaToWin);
    }
}

#endif // AOC22B_H
