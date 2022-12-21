
#include "ByteBuf.h"

#include "ByteBuf.h"

namespace
{
#define INT8_BIT sizeof(int8_t)
#define INT16_BIT sizeof(int16_t)
#define INT32_BIT sizeof(int32_t)
#define INT64_BIT sizeof(int64_t)
#define FLOAT_BIT sizeof(float)
#define DOUBLE_BIT sizeof(double)
}

class ByteBufData
{
public:
    ByteBufData()
        : readerIndex(0), writerIndex(0), markedReaderIndex(0), markedWriterIndex(0)
    {}

private:
    friend class sh::ByteBuf;
    std::string originData;
    int32_t readerIndex;
    int32_t writerIndex;
    int32_t markedReaderIndex;
    int32_t markedWriterIndex;

private:
    template<class T>
    T getT(int32_t index, uint8_t max_bit) const
    {
        if (originData.length() - index < max_bit)
        {
            return 0;
        }
        T value = 0;
        for (uint8_t delta = 0; delta < max_bit; delta++)
        {
            value += (originData.at(delta + index) & 0xff) << (8 * (max_bit - delta - 1));
        }
        return value;
    }

    template<class T>
    T getTLE(int32_t index, uint8_t max_bit) const
    {
        if (originData.length() - index < max_bit)
        {
            return 0;
        }
        T value = 0;
        for (uint8_t delta = 0; delta < max_bit; delta++)
        {
            value += (originData.at(delta + index) & 0xff) << (8 * delta);
        }
        return value;
    }

    template<class T>
    T readT(uint8_t max_bit)
    {
        if (originData.length() - readerIndex < max_bit)
        {
            return 0;
        }
        T value = 0;
        for (uint8_t delta = 0; delta < max_bit; delta++)
        {
            value += (originData.at(delta + readerIndex) & 0xff) << (8 * (max_bit - delta - 1));
        }
        readerIndex += max_bit;
        return value;
    }

    template<class T>
    T readTLE(uint8_t max_bit)
    {
        if (originData.length() - readerIndex < max_bit)
        {
            return 0;
        }
        T value = 0;
        for (uint8_t delta = 0; delta < max_bit; delta++)
        {
            value += (originData.at(delta + readerIndex) & 0xff) << (8 * delta);
        }
        readerIndex += max_bit;
        return value;
    }

    template<class T>
    void setT(int32_t index, T value)
    {
        setData(index, toBinary(value));
    }

    template<class T>
    void setTLE(int32_t index, T value)
    {
        setData(index, toBinaryLE(value));
    }

    void setData(int32_t index, const std::string &data)
    {
        if (index > originData.length())
        {
            originData.append(data);
        }
        else
        {
            originData.insert(index, data);
        }
    }

    template<class T>
    void writeT(T value)
    {
        writeData(toBinary(value));
    }

    template<class T>
    void writeTLE(T value)
    {
        writeData(toBinaryLE(value));
    }

    void writeData(const std::string &data)
    {
        if (writerIndex > originData.length())
        {
            originData.append(data);
            writerIndex = (int32_t) originData.length();
        }
        else
        {
            originData.insert(writerIndex, data);
            writerIndex += (int32_t) data.length();
        }
    }

    template<class T>
    std::string toBinary(T value)
    {
        std::string result;
        int8_t max_bit = sizeof(T);
        for (uint8_t index = 0; index < max_bit; index++)
        {
            result.push_back(value >> (8 * (max_bit - index - 1)) & 0xff);
        }
        return result;
    }

    template<class T>
    std::string toBinaryLE(T value)
    {
        std::string result;
        int8_t max_bit = sizeof(T);
        for (uint8_t index = 0; index < max_bit; index++)
        {
            result.push_back(value >> (8 * index) & 0xff);
        }
        return result;
    }
};

sh::ByteBuf::ByteBuf()
{
    d = std::unique_ptr<::ByteBufData>(new ByteBufData());
}
sh::ByteBuf::ByteBuf(const char *data, int32_t size)
{
    d = std::unique_ptr<::ByteBufData>(new ByteBufData());
    d->originData.append(data, size);
    d->writerIndex += size;
}
sh::ByteBuf::ByteBuf(const std::string &data)
{
    d = std::unique_ptr<::ByteBufData>(new ByteBufData());
    d->originData.append(data);
    d->writerIndex += data.length();
}
sh::ByteBuf::ByteBuf(int32_t size, char ch)
{
    d = std::unique_ptr<::ByteBufData>(new ByteBufData());
    d->originData.append(size, ch);
    d->writerIndex += size;
}
sh::ByteBuf::ByteBuf(const sh::ByteBuf &other)
{
    d = std::unique_ptr<::ByteBufData>(new ByteBufData());
    d->originData.append(other.d->originData);
    d->readerIndex = other.d->readerIndex;
    d->writerIndex = other.d->writerIndex;
    d->markedReaderIndex = other.d->markedReaderIndex;
    d->markedWriterIndex = other.d->markedWriterIndex;
}
sh::ByteBuf::~ByteBuf() = default;

sh::ByteBuf &sh::ByteBuf::operator=(const sh::ByteBuf &other)
{
    if (&other != this)
    {
        d->originData.append(other.d->originData);
        d->readerIndex = other.d->readerIndex;
        d->writerIndex = other.d->writerIndex;
        d->markedReaderIndex = other.d->markedReaderIndex;
        d->markedWriterIndex = other.d->markedWriterIndex;
    }
    return *this;
}

std::string sh::ByteBuf::toHexString(Case type, const std::string &fill_str) const
{
    std::string result;
    for (char ch: d->originData)
    {
        if (Case::Lower == type)
        {
            result.push_back(toHexLower(ch >> 4));
            result.push_back(toHexLower(ch & 0xF));
        }
        else
        {
            result.push_back(toHexUpper(ch >> 4));
            result.push_back(toHexUpper(ch & 0xF));
        }
        result.append(fill_str);
    }
    return result;
}

const char *sh::ByteBuf::data() const noexcept
{
    return d->originData.data();
}
int32_t sh::ByteBuf::bytesBefore(char value)
{
    for (int32_t index = d->readerIndex; index < d->writerIndex; index++)
    {
        if (d->originData.at(index) == value)
        {
            return index - d->readerIndex;
        }
    }
    return -1;
}
int32_t sh::ByteBuf::bytesBefore(int32_t length, char value)
{
    return bytesBefore(d->readerIndex, length, value);
}
int32_t sh::ByteBuf::bytesBefore(int32_t index, int32_t length, char value)
{
    bool last_ok;
    int32_t second_index;
    for (; index < d->writerIndex - length; index++)
    {
        //找到第一个符合 && 剩余的长度足够
        if (d->originData.at(index) == value && (index + length) < d->writerIndex)
        {
            last_ok = true;
            //从后一个开始检查length-1个字符
            for (second_index = index + 1; second_index < (index + length); second_index++)
            {
                if (d->originData.at(second_index) != value)
                {
                    last_ok = false;
                    break;
                }
            }
            if (last_ok)
            {
                return index - d->readerIndex;
            }
        }
    }
    return -1;
}
int32_t sh::ByteBuf::capacity() const
{
    return (int32_t) d->originData.capacity();
}
sh::ByteBuf &sh::ByteBuf::capacity(int32_t new_capacity)
{
    d->originData.reserve(new_capacity);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::clear()
{
    d->readerIndex = 0;
    d->writerIndex = 0;
    return *this;
}
int32_t sh::ByteBuf::compareTo(const sh::ByteBuf &buffer) const
{
    return d->originData.compare(buffer.d->originData);
}
sh::ByteBuf sh::ByteBuf::copy() const
{
    return sh::ByteBuf(d->originData);
}
sh::ByteBuf sh::ByteBuf::copy(int32_t index, int32_t length) const
{
    return sh::ByteBuf(d->originData.substr(index, length));
}
sh::ByteBuf &sh::ByteBuf::discardReadBytes()
{
    d->originData = d->originData.substr(d->readerIndex + 1);
    d->readerIndex = 0;
    d->writerIndex = 0;
    return *this;
}

int32_t sh::ByteBuf::indexOf(int32_t fromIndex, int32_t toIndex, char value) const
{
    for (int32_t index = fromIndex; index < d->originData.length() && index < toIndex; index++)
    {
        if (d->originData.at(index) == value)
        {
            return index;
        }
    }
    return -1;
}
sh::ByteBuf sh::ByteBuf::setIndex(int32_t readerIndex, int32_t writerIndex)
{
    if (d->originData.empty())
    {
        return *this;
    }
    d->readerIndex = readerIndex;
    d->writerIndex = writerIndex;
    if (d->readerIndex >= d->originData.length())
    {
        d->readerIndex = (int32_t) d->originData.length() - 1;
    }
    if (d->writerIndex > d->originData.length())
    {
        d->writerIndex = (int32_t) d->originData.length();
    }
    return *this;
}
bool sh::ByteBuf::isReadable() const
{
    return (d->writerIndex - d->readerIndex) > 0;
}
bool sh::ByteBuf::isReadable(int32_t size)
{
    return (d->writerIndex - d->readerIndex) >= size;
}
sh::ByteBuf &sh::ByteBuf::markReaderIndex()
{
    d->markedReaderIndex = d->readerIndex;
    return *this;
}
sh::ByteBuf &sh::ByteBuf::markWriterIndex()
{
    d->markedWriterIndex = d->writerIndex;
    return *this;
}
int32_t sh::ByteBuf::readerIndex() const
{
    return d->readerIndex;
}

sh::ByteBuf &sh::ByteBuf::readerIndex(int32_t reader_index)
{
    if (reader_index <= d->writerIndex)
    {
        d->readerIndex = reader_index;
    }
    return *this;
}
sh::ByteBuf &sh::ByteBuf::resetReaderIndex()
{
    d->readerIndex = d->markedReaderIndex;
    return *this;
}
int32_t sh::ByteBuf::writerIndex() const
{
    return d->writerIndex;
}
sh::ByteBuf &sh::ByteBuf::writerIndex(int32_t writer_index)
{
    if (writer_index < d->originData.length())
    {
        d->writerIndex = writer_index;
    }
    return *this;
}
sh::ByteBuf &sh::ByteBuf::resetWriterIndex()
{
    d->writerIndex = d->markedWriterIndex;
    return *this;
}
int32_t sh::ByteBuf::readableBytes() const
{
    return d->writerIndex - d->readerIndex;
}
char sh::ByteBuf::getByte(int32_t index) const
{
    if (index >= d->originData.length())
    {
        return 0;
    }
    return d->originData.at(index);
}
int32_t sh::ByteBuf::getBytes(int32_t index, char *dst) const
{
    if (index >= d->originData.length())
    {
        return 0;
    }
    std::string res = d->originData.substr(index);
    *dst = *res.data();
    return (int32_t) res.length();
}
int32_t sh::ByteBuf::getBytes(int32_t index, char *dst, int32_t length) const
{
    if (index >= d->originData.length())
    {
        return 0;
    }
    std::string res = d->originData.substr(index, length);
    *dst = *res.data();
    return (int32_t) res.length();
}
sh::ByteBuf sh::ByteBuf::getBytes(int32_t index) const
{
    if (index >= d->originData.length())
    {
        return sh::ByteBuf();
    }
    std::string res = d->originData.substr(index);
    return sh::ByteBuf(res);
}
sh::ByteBuf sh::ByteBuf::getBytes(int32_t index, int32_t length) const
{
    if (index >= d->originData.length())
    {
        return sh::ByteBuf();
    }
    std::string res = d->originData.substr(index, length);
    return sh::ByteBuf(res);
}
sh::ByteBuf sh::ByteBuf::getBytes(int32_t index, int32_t dstIndex, int32_t length) const
{
    if (index >= d->originData.length())
    {
        return sh::ByteBuf();
    }
    sh::ByteBuf data(dstIndex, 0);
    std::string res = d->originData.substr(index, length);
    data.writeBytes(res.data(), (int32_t) res.length());
    return data;
}
int8_t sh::ByteBuf::getChar(int32_t index) const
{
    if (index >= d->originData.length())
    {
        return 0;
    }
    return d->originData.at(index);
}
uint8_t sh::ByteBuf::getUnsignedChar(int32_t index) const
{
    return getChar(index);
}
int16_t sh::ByteBuf::getShort(int32_t index) const
{
    return d->getT<int16_t>(index, INT16_BIT);
}
int16_t sh::ByteBuf::getShortLE(int32_t index) const
{
    return d->getTLE<int16_t>(index, INT16_BIT);
}
uint16_t sh::ByteBuf::getUnsignedShort(int32_t index) const
{
    return d->getT<int16_t>(index, INT16_BIT);
}
uint16_t sh::ByteBuf::getUnsignedShortLE(int32_t index) const
{
    return d->getTLE<int16_t>(index, INT16_BIT);
}
int32_t sh::ByteBuf::getInt(int32_t index) const
{
    return d->getT<int32_t>(index, INT32_BIT);
}
int32_t sh::ByteBuf::getIntLE(int32_t index) const
{
    return d->getTLE<int32_t>(index, INT32_BIT);
}
uint32_t sh::ByteBuf::getUnsignedInt(int32_t index) const
{
    return d->getT<int32_t>(index, INT32_BIT);
}
uint32_t sh::ByteBuf::getUnsignedIntLE(int32_t index) const
{
    return d->getTLE<int32_t>(index, INT32_BIT);
}
int64_t sh::ByteBuf::getLong(int32_t index) const
{
    return d->getT<int64_t>(index, INT64_BIT);
}
int64_t sh::ByteBuf::getLongLE(int32_t index) const
{
    return d->getTLE<int64_t>(index, INT64_BIT);
}
uint64_t sh::ByteBuf::getUnsignedLong(int32_t index) const
{
    return d->getT<int64_t>(index, INT64_BIT);
}
uint64_t sh::ByteBuf::getUnsignedLongLE(int32_t index) const
{
    return d->getTLE<int64_t>(index, INT64_BIT);
}
float sh::ByteBuf::getFloat(int32_t index) const
{
    float result = 0.00;
    if (d->originData.length() - index < FLOAT_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; index < index + FLOAT_BIT; index++)
    {
        *(p + index) = d->originData.at(index);
    }
    return result;
}
float sh::ByteBuf::getFloatLE(int32_t index) const
{
    float result = 0.00;
    if (d->originData.length() - index < FLOAT_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; index < index + FLOAT_BIT; index++)
    {
        *(p + index) = d->originData.at(FLOAT_BIT - index - 1);
    }
    return result;
}

double sh::ByteBuf::getDouble(int32_t index) const
{
    double result = 0.00;
    if (d->originData.length() - index < DOUBLE_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; index < index + DOUBLE_BIT; index++)
    {
        *(p + index) = d->originData.at(index);
    }
    return result;
}
double sh::ByteBuf::getDoubleLE(int32_t index) const
{
    double result = 0.00;
    if (d->originData.length() - index < DOUBLE_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; index < index + DOUBLE_BIT; index++)
    {
        *(p + index) = d->originData.at(DOUBLE_BIT - index - 1);
    }
    return result;
}
int8_t sh::ByteBuf::readChar()
{
    if (d->originData.length() - d->readerIndex < INT8_BIT)
    {
        return 0;
    }
    return d->originData.at(d->readerIndex++);
}
uint8_t sh::ByteBuf::readUnsignedChar()
{
    return readChar();
}
int16_t sh::ByteBuf::readShort()
{
    return d->readT<int16_t>(INT16_BIT);
}
int16_t sh::ByteBuf::readShortLE()
{
    return d->readTLE<int16_t>(INT16_BIT);
}
uint16_t sh::ByteBuf::readUnsignedShort()
{
    return d->readT<int16_t>(INT16_BIT);
}
uint16_t sh::ByteBuf::readUnsignedShortLE()
{
    return d->readTLE<int16_t>(INT16_BIT);
}
int32_t sh::ByteBuf::readInt()
{
    return d->readT<int32_t>(INT32_BIT);
}
int32_t sh::ByteBuf::readIntLE()
{
    return d->readTLE<int32_t>(INT32_BIT);
}
uint32_t sh::ByteBuf::readUnsignedInt()
{
    return d->readT<int32_t>(INT32_BIT);
}
uint32_t sh::ByteBuf::readUnsignedIntLE()
{
    return d->readTLE<int32_t>(INT32_BIT);
}
int64_t sh::ByteBuf::readLong()
{
    return d->readT<int64_t>(INT64_BIT);
}
int64_t sh::ByteBuf::readLongLE()
{
    return d->readTLE<int64_t>(INT64_BIT);
}
uint64_t sh::ByteBuf::readUnsignedLong()
{
    return d->readT<int64_t>(INT64_BIT);
}
uint64_t sh::ByteBuf::readUnsignedLongLE()
{
    return d->readTLE<int64_t>(INT64_BIT);
}
sh::ByteBuf sh::ByteBuf::readBytes(int32_t length)
{
    std::string data = d->originData.substr(d->readerIndex, length);
    d->readerIndex += (int32_t) data.length();
    return sh::ByteBuf(data);
}
double sh::ByteBuf::readDouble()
{
    double result = 0.00;
    if (d->originData.length() - d->readerIndex < DOUBLE_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; d->readerIndex < d->readerIndex + DOUBLE_BIT; d->readerIndex++)
    {
        *(p + d->readerIndex) = d->originData.at(d->readerIndex);
    }
    return result;
}
double sh::ByteBuf::readDoubleLE()
{
    double result = 0.00;
    if (d->originData.length() - d->readerIndex < DOUBLE_BIT)
    {
        return 0.00;
    }
    char *p = (char *) &result;
    for (; d->readerIndex < d->readerIndex + DOUBLE_BIT; d->readerIndex++)
    {
        *(p + d->readerIndex) = d->originData.at(DOUBLE_BIT - d->readerIndex - 1);
    }
    return result;
}
sh::ByteBuf &sh::ByteBuf::skipBytes(int32_t length)
{
    d->readerIndex += length;
    if (d->readerIndex > d->originData.length())
    {
        d->readerIndex = (int32_t) d->originData.length();
    }
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setChar(int32_t index, int8_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedChar(int32_t index, uint8_t value)
{
    d->setT(index, value);
    return *this;
}

sh::ByteBuf &sh::ByteBuf::setShort(int32_t index, int16_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setShortLE(int32_t index, int16_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedShort(int32_t index, uint16_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedShortLE(int32_t index, uint16_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setInt(int32_t index, int32_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setIntLE(int32_t index, int32_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedInt(int32_t index, uint32_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedIntLE(int32_t index, uint32_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setLong(int32_t index, int64_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setLongLE(int32_t index, int64_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedLong(int32_t index, uint64_t value)
{
    d->setT(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setUnsignedLongLE(int32_t index, uint64_t value)
{
    d->setTLE(index, value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setBytes(int32_t index, const ByteBuf &buf)
{
    d->setData(index, buf.data());
    return *this;
}
sh::ByteBuf &sh::ByteBuf::setBytes(int32_t index, const char *data, int32_t size)
{
    d->setData(index, std::string(data, size));
    return *this;
}

sh::ByteBuf &sh::ByteBuf::setZero(int32_t index, int32_t length)
{
    d->setData(index, std::string(length, 0));
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeChar(int8_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedChar(uint8_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeShort(int16_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeShortLE(int16_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedShort(uint16_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedShortLE(uint16_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeInt(int32_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeIntLE(int32_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedInt(uint32_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedIntLE(uint32_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeLong(int64_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeLongLE(int64_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedLong(uint64_t value)
{
    d->writeT(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeUnsignedLongLE(uint64_t value)
{
    d->writeTLE(value);
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeBytes(const ByteBuf &buf)
{
    d->writeData(buf.data());
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeBytes(const char *data, int32_t size)
{
    d->writeData(std::string(data, size));
    return *this;
}
sh::ByteBuf &sh::ByteBuf::writeZero(int32_t length)
{
    d->writeData(std::string(length, 0));
    return *this;
}
char sh::ByteBuf::toHexLower(uint32_t value) noexcept
{
    return "0123456789abcdef"[value & 0xF];
}
char sh::ByteBuf::toHexUpper(uint32_t value) noexcept
{
    return "0123456789ABCDEF"[value & 0xF];
}
int32_t sh::ByteBuf::fromHex(uint32_t c) noexcept
{
    return ((c >= '0') && (c <= '9')) ? int32_t(c - '0') :
           ((c >= 'A') && (c <= 'F')) ? int32_t(c - 'A' + 10) :
           ((c >= 'a') && (c <= 'f')) ? int32_t(c - 'a' + 10) :
           /* otherwise */              -1;
}
sh::ByteBuf sh::ByteBuf::fromHex(const std::string &hexEncoded) noexcept
{
    sh::ByteBuf data;
    char ch = 0x00;
    for (int32_t index = 0; index < hexEncoded.length() - 1; index++)
    {
        ch |= (char) fromHex(hexEncoded.at(index++)) << 4;
        ch |= (char) fromHex(hexEncoded.at(index));
        data.writeChar(ch);
        ch = 0x00;
    }
    return data;
}