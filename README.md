# COFFPacker

COFFPacker is a small C++ utility for packing arguments into the format expected by TrustedSec's [COFFLoader](https://github.com/trustedsec/COFFLoader). It mirrors the behaviour of the `beacon_generate.py` script so the packing logic can be embedded in other C++ projects.

## Building

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

This produces the `CoffPacker` static library and a `testsCoffPacker` example executable.

## Usage

`CoffPacker::process` takes a space separated list of values and a format string (`z` for string, `Z` for wide string, `s` for short, `i` for int). The packed result is returned as a hexadecimal string.

```cpp
CoffPacker packer;
std::string output;
packer.process("Y:\\COFFLoader 1", "Zs", output);
```
