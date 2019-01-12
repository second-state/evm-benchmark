#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

template<typename T, typename U> bool contain(T &&container, U &&val)
{
    return std::find(begin(container), end(container), val) != end(container);
}

std::vector<uint8_t> hex2Uint8Vec(const std::string &hexs);