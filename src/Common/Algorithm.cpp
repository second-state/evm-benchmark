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
    std::size_t begin = hexs.find("0x")==0 ? 2 : 0;

    for(std::size_t i = begin ; i < len; i += 2)
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

int64_t hex2int64(const std::string &hexs)
{
    int64_t res = 0;
    std::size_t len = hexs.size();
    std::size_t begin = hexs.find("0x")==0 ? 2 : 0;

    for(std::size_t i = begin ; i < len; ++i)
    {
        assert( std::isxdigit(hexs[i]) );
        res = res * 16 + hex2Uint(hexs[i]);
    }
    return res;
}