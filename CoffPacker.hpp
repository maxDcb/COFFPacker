#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <random>
#include <vector>
#include <thread>


typedef unsigned char uint8_t;
typedef std::vector<uint8_t> ByteBuffer;


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

