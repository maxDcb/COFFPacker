#include <string>
#include <iostream>

#include "CoffPacker.hpp"

int main()
{
    CoffPacker coffPacker;

    std::string arg = "Y:\\COFFLoader";
    std::string format = "z";
    std::string out;
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "Y:\\COFFLoader";
    format = "Z";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "42";
    format = "s";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "525";
    format = "i";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "Y:\\COFFLoader 1";
    format = "Zs";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "B:\\ 1";
    format = "Zs";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    arg = "B:\\ 1";
    format = "Zs";
    out.clear();
    coffPacker.process(arg, format, out);
    std::cout << out << std::endl;

    return 0;
}
