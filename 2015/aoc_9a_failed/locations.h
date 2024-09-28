#ifndef LOCATIONS_H
#define LOCATIONS_H

#include <string_view>
#include <vector>

namespace locations
{
    bool XOR(std::string_view origin, std::string_view destination, std::string_view location1, std::string_view location2)
    {
        if (origin == location1)
            return (origin == location1) != (destination == location2);
        else
            return (origin == location2) != (destination == location1);
    }

    bool AND(std::string_view origin, std::string_view destination, std::string_view location1, std::string_view location2)
    {
        if (origin == location1)
            return (origin == location1) == (destination == location2);
        else
            return (origin == location2) == (destination == location1);
    }

    std::vector<std::string_view> getList(const auto& routes, std::string_view uniqueLocation, int routeIndex)
    {
        std::pair<std::string_view, int> key {uniqueLocation, routeIndex};
        std::vector<std::string_view> routeList {uniqueLocation};
        for (const auto& [key, value] : routes.at(key))
        {
            routeList.push_back(value.first);
        }
        return routeList;
    }
}

#endif // LOCATIONS_H
