#ifndef BINARY_STREAM_READER_HPP
#define BINARY_STREAM_READER_HPP

#include <cstdint>
#include <ios>
#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>

class set_flags;
class set_width;

class BinaryStreamReader
{
public:
    explicit BinaryStreamReader(std::istream& _stream, uint32_t _flags = READ_SIZES) : stream(_stream), flags(_flags), width(0) {}
    BinaryStreamReader& operator>>(bool& val);
    BinaryStreamReader& operator>>(char& val);
    BinaryStreamReader& operator>>(unsigned char& val);
    BinaryStreamReader& operator>>(int16_t& val);
    BinaryStreamReader& operator>>(uint16_t& val);
    BinaryStreamReader& operator>>(int& val);
    BinaryStreamReader& operator>>(unsigned int& val);
    BinaryStreamReader& operator>>(float& val);
    BinaryStreamReader& operator>>(std::string& val);
    BinaryStreamReader& operator>>(BinaryStreamReader& (*pf)(BinaryStreamReader&));
    void SetFlags(uint32_t _flags) { flags = _flags; }
    void SetWidth(uint32_t _width) { width = _width; }
    uint32_t Flags() const { return flags; }
    uint32_t Width() const { return width; }
    void SetMaxStringSize(uint32_t size) { max_string_size = size; }
    uint32_t GetMaxStringSize() const { return max_string_size; }
    void SetMaxVectorSize(uint32_t size) { max_vector_size = size; }
    uint32_t GetMaxVectorSize() const { return max_vector_size; }
    void SetInternalStreamState(std::ios_base::iostate state) { stream.setstate(state); }
    bool Ok() const { return !stream.fail(); }
    enum
    {
        NO_READ_STRING_SIZES = 0,
        NO_READ_VECTOR_SIZES = 0,
        NO_READ_SIZES = 0,
        READ_STRING_SIZES = 1,
        READ_VECTOR_SIZES = 2,
        READ_SIZES = 3,
    };

private:
    std::istream& stream;
    uint32_t flags;
    uint32_t width;
    uint32_t max_string_size = -1;
    uint32_t max_vector_size = -1;
};

template<typename VecType>
BinaryStreamReader& operator>>(BinaryStreamReader& cs, std::vector<VecType>& vec)
{
    uint32_t size;

    if (cs.Flags() & BinaryStreamReader::READ_VECTOR_SIZES)
        cs >> size;
    else
        size = vec.size();

    if (size > cs.GetMaxVectorSize())
    {
        cs.SetInternalStreamState(std::ios_base::failbit);
        return cs;
    }

    vec.resize(size);
    VecType el;
    for (uint32_t i = 0; i < size; i++)
    {
        cs >> el;
        vec[i] = el;
    }

    return cs;
}

class set_width
{
public:
    explicit set_width(uint32_t _width): width(_width) {}
    uint32_t get_width() const { return width; }
private:
    uint32_t width;
};

class set_flags
{
public:
    explicit set_flags(uint32_t _flags): flags(_flags) {}
    uint32_t get_flags() const { return flags; }
private:
    uint32_t flags;
};

BinaryStreamReader& operator>>(BinaryStreamReader& cs, const set_width& width);
BinaryStreamReader& operator>>(BinaryStreamReader& cs, const set_flags& flags);

#endif
