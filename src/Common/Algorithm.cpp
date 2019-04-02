#include <Common/Algorithm.h>

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <vector>

static inline uint8_t hex2Uint(char c)
{
    if( c <= '9' )
        return c - '0';
    if( c <= 'Z' )
        return c - 'A' + 10;
    return c - 'a' + 10;
}

std::vector<uint8_t> hex2Uint8Vec(const std::string &hexs)
{
    std::vector<uint8_t> res;
    assert( hexs.size() %2 == 0 );

    std::size_t len = hexs.size();

    for(std::size_t i = 0 ; i < len; i += 2)
    {
        assert( std::isxdigit(hexs[i]) && std::isxdigit(hexs[i+1]) );
        res.emplace_back( hex2Uint(hexs[i])*16 + hex2Uint(hexs[i+1]) );
    }

    return res;
}

std::string uint8Arr2hexString(const uint8_t *arr, size_t n)
{
    std::string res;
    for(size_t i=0; i<n; ++i)
    {
        uint8_t tmp[2] = {uint8_t(arr[i]/16), uint8_t(arr[i]%16)};
        for(auto u:tmp)
        {
            char c = u<10 ? u+'0' : u-10+'a';
            res += c;
        }
    }
    return res;
}