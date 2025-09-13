#pragma once

#include <string>
#include <vector>
#include <cstdint>

using ByteBuffer = std::vector<std::uint8_t>;


class CoffPacker
{

public:
	CoffPacker();
	~CoffPacker();

	int process(const std::string& input, const std::string& format, std::string& output);

private:
    int addWString(const std::string& input);
    int addString(const std::string& input);
    int addShort(const std::string& input);
    int addInt(const std::string& input);

    ByteBuffer m_data;
    std::string m_buf;

};

