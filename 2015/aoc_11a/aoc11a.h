#ifndef AOC11A_H
#define AOC11A_H

#include <string>
#include <unordered_set>

namespace aoc11a
{
    std::string incrementPassword(std::string_view oldPassword)
    {
        std::string newPassword {oldPassword};
        int oldPasswordSize {static_cast<int>(oldPassword.size())};
        for (int i{oldPasswordSize - 1}; i >= 0; --i)
        {
            char newLetter {static_cast<char>(newPassword[i] + 1)};
            if (newLetter <= 'z')
            {
                newPassword[i] = newLetter;
                return newPassword;
            }
            else
            {
                newPassword[i] = 'a';
            }
        }
        return newPassword;
    }

    bool isPasswordValid(std::string_view password)
    {
        std::unordered_set<char> doubleLetters {};
        char previousLetter {};
        int consecutiveLetterCount {1};

        int letterStraightCount {1};
        bool isThreeLetterStraight {false};

        bool initialLoop {true};

        for (auto letter : password)
        {
            // guard clause for excluded letters
            if ((letter == 'i') | (letter == 'o') | (letter == 'l'))
                return false;
            
            // initial loop has no previous letter, so skip most of loop
            if (initialLoop)
            {
                previousLetter = letter;
                initialLoop = false;
                continue;
            }

            if (letter == previousLetter + 1)
                ++letterStraightCount;
            else
                letterStraightCount = 1;

            if (letter == previousLetter)
                ++consecutiveLetterCount;
            else
            {
                previousLetter = letter;
                consecutiveLetterCount = 1;
            }

            if ((!isThreeLetterStraight) && (letterStraightCount >= 3))
                isThreeLetterStraight = true;

            if (consecutiveLetterCount == 2)
                doubleLetters.insert(letter);
            
            if ((doubleLetters.size() >= 2) && isThreeLetterStraight)
                return true;
        }

        if ((doubleLetters.size() >= 2) && isThreeLetterStraight)
            return true;
        
        return false;
    }

    std::string parseAndGetNextPassword(std::string_view input)
    {
        std::string newPass {input};
        while (!(isPasswordValid(newPass)))
        {
            newPass = incrementPassword(newPass);
        }
        return newPass;
    }
}

#endif // AOC11A_H
