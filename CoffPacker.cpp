#include "CoffPacker.hpp"

#include <cstring>

#include "struct_endian.h"
#include "struct.h"


void static inline splitString(std::string list, const std::string& delimiter, std::vector<std::string>& splitedList)
{
	size_t pos = 0;
	std::string token;
	while ((pos = list.find(delimiter)) != std::string::npos)
	{
		token = list.substr(0, pos);
		splitedList.push_back(token);
		list.erase(0, pos + delimiter.length());
	}
	splitedList.push_back(list);
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
        int len = struct_pack(m_buf.data(), "<L", m_data.size());
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
    std::memset(fmt, 0, sizeof(fmt));
    sprintf(fmt, "<%ds", (int)input.size()*2+2);

    ByteBuffer buffer;
    int len = struct_pack(m_buf.data(), fmt, arg.data());
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    ByteBuffer bufferSize;
    len = struct_pack(m_buf.data(), "<L", buffer.size());
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
    std::memset(fmt, 0, sizeof(fmt));
    sprintf(fmt, "<%ds", (int)input.size()+1);

    ByteBuffer buffer;
    int len = struct_pack(m_buf.data(), fmt, arg.data());
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    ByteBuffer bufferSize;
    len = struct_pack(m_buf.data(), "<L", buffer.size());
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
        val = (short)std::stoi(input);
    }
    catch (...)
    {
        return -1;
    }

    ByteBuffer buffer;
    int len = struct_pack(m_buf.data(), "<h", val);
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
        val = (int)std::stoi(input);
    }
    catch (...)
    {
        return -1;
    }

    ByteBuffer buffer;
    int len = struct_pack(m_buf.data(), "<i", val);
    for(int i=0; i<len; i++)
        buffer.push_back(m_buf[i]);

    m_data.insert(m_data.end(), buffer.begin(), buffer.end());   

    return 0;
}


    // hexlify:
    // https://gist.github.com/userx007/9020ecc81a33b304a081442512149356

    // struct_pack:
    // https://github.com/svperbeast/struct/blob/master/test/struct_test.cpp

    // #include "struct_endian.h"
    // #include "struct.h"

    // typedef unsigned char uint8_t;
    // typedef std::vector<uint8_t> ByteBuffer;

    // std::string hexlify(const ByteBuffer &InBuffer)
    // {
    //     const char hex[] = "0123456789ABCDEF";
    //     std::string OutBuffer(InBuffer.size() * 2 + 1, '\0');

    //     for(size_t i = 0; i < InBuffer.size(); ++i)
    //     {
    //         char* dest = &OutBuffer[i * 2];
    //         *dest++ = hex[ ((InBuffer[i] >> 4) & 0xF) ];
    //         *dest++ = hex[ (InBuffer[i] & 0xF) ];
    //     }

    //     return OutBuffer;
    // }

    // if(1)
    // {
    //     // addWString Y:\COFFLoader
    //     std::string addString = "200000001c00000059003a005c0043004f00460046004c006f0061006400650072000000";
    //                          // "        1C00000059003A005C0043004F00460046004C006F0061006400650072000000"

    //     std::cout << addString << std::endl;

    //     char buf[512];

    //     // add string
    //     ByteBuffer data;
    //     {

    //         std::wstring arg = L"Y:\\COFFLoader";
    //         char fmt[32];
    //         sprintf(fmt, "<%ds", (int)arg.size()*2+2);

    //         std::cout << fmt << std::endl;

    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, fmt, arg.data());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         std::cout << "len " << len << std::endl;

    //         ByteBuffer bufferSize;
    //         len = struct_pack(buf, "<L", buffer.size());
    //         for(int i=0; i<len; i++)
    //             bufferSize.push_back(buf[i]);

    //         data.insert(data.end(), bufferSize.begin(), bufferSize.end());
    //         data.insert(data.end(), buffer.begin(), buffer.end());
    //     }

    //     ByteBuffer finalBuffer;
    //     {
    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, "<L", data.size());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());
    //         finalBuffer.insert(finalBuffer.end(), data.begin(), data.end());

    //         std::cout << "len " << len << std::endl;
    //     }

        
    //     std::string test = hexlify(finalBuffer);
    //     std::cout << test << std::endl;      
    // }

    // if(1)
    // {
    //     // addString Y:\COFFLoader
    //     std::string addString = "120000000e000000593a5c434f46464c6f6164657200";
    //                          // "120000000E000000593A5C434F46464C6F6164657200"

    //             std::cout << addString << std::endl;

    //     char buf[512];

    //     // add string
    //     ByteBuffer data;
    //     {

    //         std::string arg = "Y:\\COFFLoader";
    //         char fmt[32];
    //         sprintf(fmt, "<%ds", (int)arg.size()+1);

    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, fmt, arg.data());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         std::cout << "len " << len << std::endl;

    //         ByteBuffer bufferSize;
    //         len = struct_pack(buf, "<L", buffer.size());
    //         for(int i=0; i<len; i++)
    //             bufferSize.push_back(buf[i]);

    //         data.insert(data.end(), bufferSize.begin(), bufferSize.end());
    //         data.insert(data.end(), buffer.begin(), buffer.end());
    //     }

    //     ByteBuffer finalBuffer;
    //     {
    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, "<L", data.size());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());
    //         finalBuffer.insert(finalBuffer.end(), data.begin(), data.end());

    //         std::cout << "len " << len << std::endl;
    //     }

        
    //     std::string test = hexlify(finalBuffer);
    //     std::cout << test << std::endl;      
    // }

    // // add short
    // if(1)
    // {
    //     // addshort 1
    //     std::string addString = "020000000100";

    //     std::cout << addString << std::endl;

    //     char buf[512];

    //     ByteBuffer data;
    //     {
    //         ByteBuffer buffer;
    //         short valu = 1;
    //         int len = struct_pack(buf, "<h", valu);
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         data.insert(data.end(), buffer.begin(), buffer.end());

    //         std::cout << "len " << len << std::endl;
    //     }

    //     ByteBuffer finalBuffer;
    //     {
    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, "<L", data.size());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());
    //         finalBuffer.insert(finalBuffer.end(), data.begin(), data.end());

    //         std::cout << "len " << len << std::endl;
    //     }

        
    //     std::string test = hexlify(finalBuffer);
    //     std::cout << test << std::endl;
        
    // }

    // // add int
    // if(1)
    // {
    //     // addint 45
    //     std::string addString = "040000002d000000";

    //     std::cout << addString << std::endl;

    //     char buf[512];

    //     ByteBuffer data;
    //     {
    //         ByteBuffer buffer;
    //         short valu = 45;
    //         int len = struct_pack(buf, "<i", valu);
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         data.insert(data.end(), buffer.begin(), buffer.end());

    //         std::cout << "len " << len << std::endl;
    //     }

    //     ByteBuffer finalBuffer;
    //     {
    //         ByteBuffer buffer;
    //         int len = struct_pack(buf, "<L", data.size());
    //         for(int i=0; i<len; i++)
    //             buffer.push_back(buf[i]);

    //         finalBuffer.insert(finalBuffer.end(), buffer.begin(), buffer.end());
    //         finalBuffer.insert(finalBuffer.end(), data.begin(), data.end());

    //         std::cout << "len " << len << std::endl;
    //     }

        
    //     std::string test = hexlify(finalBuffer);
    //     std::cout << test << std::endl;
        
    // }