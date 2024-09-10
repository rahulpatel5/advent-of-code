#ifndef ESCAPE_H
#define ESCAPE_H

#include <string_view>

namespace escape
{
    bool isEscapeSequence(std::string_view line, auto index)
    {
        // hard-coding character escape sequence of \\ or \"
        return ((line[index] == '\\') && ((line[index + 1] == '\\') || (line[index + 1] == '\"')));
    }

    bool isHexadecimal(char c)
    {
        return (((c >= '0') && (c <= '9')) || ((c >= 'a') && (c <= 'f')));
    }

    bool isHexadecimalEscape(std::string_view line, auto index)
    {
        if (index >= line.size() - 3)
            return false;
        // catch sequences e.g. a\\x12, which aren't hexadecimal
        if ((index >= 2) && (line[index - 1] == '\\') && (line[index - 2] != '\\'))
            return false;
        
        return ((line[index] == '\\') && (line[index + 1] == 'x') && isHexadecimal(line[index + 2]) && isHexadecimal(line[index + 3]));
    }

    bool isEscapePair(std::string_view line, auto index)
    {
        if (index >= line.size() - 3)
            return false;
        
        // avoid double-counting sequences of \\\\ or \\\"
        // this doesn't work for longer sequences, e.g. \\\\\"
        return ((line[index] == '\\') && (line[index + 1] == '\\') && (isHexadecimalEscape(line, index + 2) || isEscapeSequence(line, index + 2)));
    }
}

#endif // ESCAPE_H
