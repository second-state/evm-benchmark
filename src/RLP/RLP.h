#pragma once
#include <vector>
#include <cstdint>
using std::vector;
struct RLPitem
{
	bool isStr;
	vector<uint8_t> str;
	vector<RLPitem> L;
	void setStr(const vector<uint8_t> &m_str)
	{
		str = m_str;
		isStr = true;
		L.clear();
	}
	void setList(const vector<RLPitem> &m_L)
	{
		L = m_L;
		isStr = false;
		str.clear();
	}
	vector<uint8_t> encode() const;
};