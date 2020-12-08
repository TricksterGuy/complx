#include "BinaryStreamReader.hpp"

#include <cstring>
#include <vector>

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#ifdef _WIN32
#include <winsock2.h>
#endif
#endif

BinaryStreamReader& BinaryStreamReader::operator>>(bool& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(char& val)
{
    stream.read(&val, sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(unsigned char& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(int16_t& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(uint16_t& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(int& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(unsigned int& val)
{
    stream.read(reinterpret_cast<char*>(&val), sizeof(val));
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(float& val)
{
    union
    {
        float f;
        int32_t i;
    } convert;

    stream.read(reinterpret_cast<char*>(&convert.i), sizeof(int32_t));
    val = convert.f;
    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(std::string& val)
{
    uint32_t effSize = width;
    if ((flags & BinaryStreamReader::READ_STRING_SIZES) && width == 0)
        (*this) >> effSize;

    if (effSize > max_string_size)
    {
        stream.setstate(std::ios_base::failbit);
        return *this;
    }

    std::vector<char> buffer(effSize);
    stream.read(buffer.data(), effSize);
    val.assign(buffer.data(), effSize);

    width = 0;

    return *this;
}

BinaryStreamReader& BinaryStreamReader::operator>>(BinaryStreamReader& (*pf)(BinaryStreamReader&))
{
    return pf(*this);
}

BinaryStreamReader& operator>>(BinaryStreamReader& cs, const set_width& width)
{
    cs.SetWidth(width.get_width());
    return cs;
}

BinaryStreamReader& operator>>(BinaryStreamReader& cs, const set_flags& flags)
{
    cs.SetFlags(flags.get_flags());
    return cs;
}
