#include "CoffPacker.hpp"

#include <cstdio>
#include <iostream>

#include "struct_endian.h"
#include "struct.h"


static inline void splitString(const std::string& list, const std::string& delimiter, std::vector<std::string>& splitedList)
{
    size_t start = 0;
    size_t end;
    while ((end = list.find(delimiter, start)) != std::string::npos)
    {
        splitedList.emplace_back(list.substr(start, end - start));
        start = end + delimiter.length();
    }
    splitedList.emplace_back(list.substr(start));
}


std::string hexlify(const ByteBuffer &InBuffer)
{
    const char hex[] = "0123456789ABCDEF";
    std::string OutBuffer(InBuffer.size() * 2 + 1, '\0');

    for(size_t i = 0; i < InBuffer.size(); ++i)
    {
        char* dest = &OutBuffer[i * 2];
        *dest++ = hex[ ((InBuffer[i] >> 4) & 0xF) ];
        *dest++ = hex[ (InBuffer[i] & 0xF) ];
    }

    return OutBuffer;
}


CoffPacker::CoffPacker()
{
    m_buf.resize(2048,'0');
}


CoffPacker::~CoffPacker()
{

}


int CoffPacker::process(const std::string& input, const std::string& format, std::string& output)
{
    std::vector<std::string> splitedList;
    splitString(input, " ", splitedList);

    if(splitedList.size()!=format.size())
        return -1;

    for(int i=0; i<format.size(); i++)
    {
        if(format[i]=='z')
        {
            addString(splitedList[i]);
        }
        else if(format[i]=='Z')
        {
            addWString(splitedList[i]);
        }
        else if(format[i]=='i')
        {
            addInt(splitedList[i]);
        }
        else if(format[i]=='s')
        {
            addShort(splitedList[i]);
        }
    }

    ByteBuffer finalBuffer;
    {
        ByteBuffer buffer;
        int len = struct_pack((void*)m_buf.data(), "<L", m_data.size());
        for(int i=0; i<len; i++)
            buffer.push_back(m_buf[i]);

        finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());
        finalBuffer.insert(finalBuffer.end(), m_data.begin(), m_data.end());
    }

    output = hexlify(finalBuffer);
    m_data.clear();

    return 0;
}


int CoffPacker::addWString(const std::string& input)
{
    std::wstring arg(input.begin(), input.end());
    arg.push_back('\0');
    arg.push_back('\0');
    char fmt[32];
    std::snprintf(fmt, sizeof(fmt), "<%ds", static_cast<int>(input.size()) * 2 + 2);

    ByteBuffer buffer;
    int len = struct_pack((void*)m_buf.data(), fmt, arg.data());
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    ByteBuffer bufferSize;
    len = struct_pack((void*)m_buf.data(), "<L", buffer.size());
    for(int i=0; i<len; i++)
        bufferSize.push_back(m_buf[i]);

    m_data.insert(m_data.end(), bufferSize.begin(), bufferSize.end());
    m_data.insert(m_data.end(), buffer.begin(), buffer.end());

    return 0;
}


int CoffPacker::addString(const std::string& input)
{
    std::string arg(input.begin(), input.end());
    arg.push_back('\0');
    arg.push_back('\0');
    char fmt[32];
    std::snprintf(fmt, sizeof(fmt), "<%ds", static_cast<int>(input.size()) + 1);

    ByteBuffer buffer;
    int len = struct_pack((void*)m_buf.data(), fmt, arg.data());
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    ByteBuffer bufferSize;
    len = struct_pack((void*)m_buf.data(), "<L", buffer.size());
    for(int i=0; i<len; i++)
        bufferSize.push_back(m_buf[i]);

    m_data.insert(m_data.end(), bufferSize.begin(), bufferSize.end());
    m_data.insert(m_data.end(), buffer.begin(), buffer.end());

    return 0;
}


int CoffPacker::addShort(const std::string& input)
{
    short val=0;
    try
    {
        val = static_cast<short>(std::stoi(input));
    }
    catch (const std::exception& e)
    {
        std::cerr << "addShort stoi error: " << e.what() << '\n';
        return -1;
    }

    ByteBuffer buffer;
    int len = struct_pack((void*)m_buf.data(), "<h", val);
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    m_data.insert(m_data.end(), buffer.begin(), buffer.end());    

    return 0;
}


int CoffPacker::addInt(const std::string& input)
{
    int val=0;
    try
    {
        val = std::stoi(input);
    }
    catch (const std::exception& e)
    {
        std::cerr << "addInt stoi error: " << e.what() << '\n';
        return -1;
    }

    ByteBuffer buffer;
    int len = struct_pack((void*)m_buf.data(), "<i", val);
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    m_data.insert(m_data.end(), buffer.begin(), buffer.end());

    return 0;
}
