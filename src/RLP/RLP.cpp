#include <vector>
#include <cstdint>
#include <RLP/RLP.h>
using std::vector;
vector<uint8_t> to_binary(size_t x)
{
    vector<uint8_t> res;
    while (x)
    {
        res.emplace_back(x % 256);
        x /= 256;
    }
    return res;
}
vector<uint8_t> encode_length(size_t L, size_t offset)
{
    if (L < 56)
        return vector<uint8_t>(1, L + offset);
    auto BL = to_binary(L);
    auto res = vector<uint8_t>(1, BL.size() + offset + 55);
    res.insert(res.end(), BL.begin(), BL.end());
    return res;
}

vector<uint8_t> RLPitem::encode() const
{
    if (isStr)
    {
        if (str.size() == 1 && str[0] < 128)
            return str;
        auto res = encode_length(str.size(), 128);
        res.insert(res.end(), str.begin(), str.end());
        return res;
    }
    else
    {
        vector<uint8_t> output;
        for (const RLPitem &item : L)
        {
            auto res = item.encode();
            output.insert(output.end(), res.begin(), res.end());
        }
        auto res = encode_length(output.size(), 192);
        res.insert(res.end(), output.begin(), output.end());
        return res;
    }
}