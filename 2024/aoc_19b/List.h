#ifndef LIST_H
#define LIST_H

#include <vector>
#include <string>

class List
{
private:
    std::vector<std::vector<std::string>> m_lists {};

public:
    void trim(const std::vector<size_t>& l)
    {
        for (auto it {l.rbegin()}; it != l.rend(); ++it)
            m_lists.erase(m_lists.begin() + *it);
    }

    void add(const std::vector<std::string>& l)
    {
        for (size_t i{0}; i < l.size(); ++i)
        {
            m_lists[i].push_back(l[i]);
        }
    }

    void push(const std::vector<std::string>& l) { m_lists.push_back(l); }

    void push_many(const List& l)
    {
        for (const std::vector<std::string>& i : l)
            m_lists.push_back(i);
    }

    std::string combine_str(size_t index)
    {
        std::string s {};
        for (const std::string& st : m_lists[index])
            s.append(st);
        return s;
    }

    size_t size() { return m_lists.size(); }

    size_t str_size(size_t index)
    {
        size_t sz {0};
        for (std::string e : m_lists[index])
            sz += e.size();
        return sz;
    }

    std::string last(size_t index) { return m_lists[index].back(); }

    auto begin() -> decltype(m_lists.begin()) { return m_lists.begin(); }
    auto end() -> decltype(m_lists.end()) { return m_lists.end(); }
    auto begin() const -> decltype(m_lists.begin()) { return m_lists.begin(); }
    auto end() const -> decltype(m_lists.end()) { return m_lists.end(); }

    std::vector<std::string>& operator[](size_t index) { return m_lists[index]; }
    const std::vector<std::string>& operator[](size_t index) const { return m_lists[index]; }
};

#endif // LIST_H
