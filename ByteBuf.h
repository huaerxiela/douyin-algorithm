#ifndef SH_BYTE_BUF_H
#define SH_BYTE_BUF_H

#include <string>
#include <memory>

class ByteBufData;
namespace sh
{

enum Case
{
    Lower = 0,  //小写
    Upper,      //大写
};
/**
 * ByteBuf封装，内置readerIndex和writerIndex基数，用于记录当前读取和写入的位置
 *
 * +-------------------+------------------+------------------+
 * | discardable bytes |  readable bytes  |  writable bytes  |
 * |                   |     (CONTENT)    |                  |
 * +-------------------+------------------+------------------+
 * |                   |                  |                  |
 * 0      <=      readerIndex   <=   writerIndex    <=    capacity
 *
 * @author: sherlock_lht
 */
class ByteBuf
{
public:
    static char toHexLower(uint32_t value) noexcept;
    static char toHexUpper(uint32_t value) noexcept;
    static int32_t fromHex(uint32_t c) noexcept;
    static ByteBuf fromHex(const std::string &hexEncoded) noexcept;

public:
    explicit ByteBuf();
    explicit ByteBuf(const char *data, int32_t size = -1);
    explicit ByteBuf(const std::string &data);
    explicit ByteBuf(int32_t size, char ch);
    ByteBuf(const ByteBuf &other);
    virtual ~ByteBuf();

    ByteBuf &operator=(const ByteBuf &other);

    /**
     * 转换成16进制字符串
     */
    std::string toHexString(Case type = Case::Lower, const std::string &fill_str = "") const;

    /**
     * 返回现有的字节数组
     */
    const char *data() const noexcept;

    /**
     * 从readerIndex开始（包括readerIndex），到writerIndex结束（不包括writerIndex），找到buff内第一次出现的value的位置
     * 该方法不会改变readerIndex和writerIndex
     *
     * @return value出现的位置到readerIndex之间的字节数，如果没找到则返回-1
     */
    int32_t bytesBefore(char value);

    /**
     * 从readerIndex开始（包括readerIndex），到writerIndex结束（不包括writerIndex），找到buff内第一次出现连续长度为length的value的位置
     * 该方法不会改变readerIndex和writerIndex
     *
     * @return value出现的位置到readerIndex之间的字节数，如果没找到则返回-1
     */
    int32_t bytesBefore(int32_t length, char value);

    /**
     * 从index开始（包括index），到writerIndex结束（不包括writerIndex），找到buff内第一次出现连续长度为length的value的位置
     * 该方法不会改变readerIndex和writerIndex
     *
     * @return value出现的位置到readerIndex之间的字节数，如果没找到则返回-1
     */
    int32_t bytesBefore(int32_t index, int32_t length, char value);

    /**
     * 返回为对象分配的存储空间大小
     */
    int32_t capacity() const;

    /**
     * 重新分配对象的存储空间大小，如果new_capacity不够存储现有的数据，则capacity保持不变
     */
    ByteBuf &capacity(int32_t new_capacity);

    /**
     * readerIndex和writerIndex全部归零，等同于setIndex(0, 0)
     */
    ByteBuf &clear();

    /**
     * 比较缓存区的数据内容，同strcmp
     */
    int32_t compareTo(const ByteBuf &buffer) const;

    /**
     * 返回对象的可读字节数据的副本，即从readerIndex开始到writerIndex结束的数据
     * 等同于 buf.copy(buf.readerIndex(), buf.readableBytes())
     * 不影响原始对象的readerIndex和writerIndex，且两个对象互不影响
     */
    ByteBuf copy() const;

    /**
     * 返回对象数据从index开始，长度的length的数据的副本
     * 不影响原始对象的readerIndex和writerIndex，且两个对象互不影响
     */
    ByteBuf copy(int32_t index, int32_t length) const;

    /**
     * 丢弃从0th到readerIndex之间的所有数据
     * 即移动readerIndex到writerIndex之间的数据到0th，并且设置readerIndex和writerIndex为0
     */
    ByteBuf &discardReadBytes();

    /**
     * 从buffer中定位第一次出现的value字符，搜索从fromIndex（包括fromIndex）到toIndex（不包括toIndex）
     * 该方法不会改变readerIndex和writerIndex
     *
     * @return value第一次出现的下表，没找到返回-1
     */
    int32_t indexOf(int32_t fromIndex, int32_t toIndex, char value) const;

    /**
     * 设置readerIndex和writerIndex，如果buffer内无数据，则无效;
     * 如果readerIndex超出了buffer内数据的长度，则readerIndex = buffer.length() - 1;
     * 如果writerIndex超出了buffer内数据的长度，则writerIndex = buffer.length()
     */
    ByteBuf setIndex(int32_t readerIndex, int32_t writerIndex);

    /**
     * 只有当this.writerIndex - this.readerIndex大于0时，返回true
     */
    bool isReadable() const;

    /**
     * 只有当this.writerIndex - this.readerIndex大于size时，返回true
     */
    bool isReadable(int32_t size);

    /**
     * 标记当前的readerIndex，后面可以使用resetReaderIndex()撤回readerIndex到标记位置，初始标记位置为0
     */
    ByteBuf &markReaderIndex();

    /**
     * 标记当前的writerIndex，后面可以使用resetWriterIndex()撤回resetWriterIndex到标记位置，初始标记位置为0
     */
    ByteBuf &markWriterIndex();

    /**
     * 返回readerIndex
     */
    int32_t readerIndex() const;

    /**
     * 设置readerIndex
     */
    ByteBuf &readerIndex(int32_t reader_index);

    /**
     * 重新定位readerIndex到标记的readerIndex位置
     */
    ByteBuf &resetReaderIndex();

    /**
     * 返回writerIndex
     */
    int32_t writerIndex() const;

    /**
     * 设置writerIndex
     */
    ByteBuf &writerIndex(int32_t writer_index);

    /**
     * 重新定位writerIndex到标记的writerIndex位置
     */
    ByteBuf &resetWriterIndex();

    /**
     * 返回当前buffer中的可读的字节数，等同于(this.writerIndex - this.readerIndex)
     */
    int32_t readableBytes() const;

    /**
     * 获取指定下标的字符
     * 该方法不会修改readerIndex和writerIndex
     *
     * @return 如果index超出buffer数据长度，返回0
     */
    char getByte(int32_t index) const;

    /**
     * 从指定的index位置，获取符合条件的字符，该方法不会修改readerIndex和writerIndex
     *
     *  @return 返回实际获取的字符数目
     */
    int32_t getBytes(int32_t index, char *dst) const;
    int32_t getBytes(int32_t index, char *dst, int32_t length) const;

    /**
     * 从指定的index位置，获取符合条件的字符，该方法不会修改readerIndex和writerIndex
     */
    ByteBuf getBytes(int32_t index) const;
    ByteBuf getBytes(int32_t index, int32_t length) const;
    ByteBuf getBytes(int32_t index, int32_t dstIndex, int32_t length) const;

    /**
     * 以下方法分别从index位置读取8/16/32/64位的数据，不会修改readerIndex和writerIndex
     */
    int8_t getChar(int32_t index) const;
    uint8_t getUnsignedChar(int32_t index) const;

    int16_t getShort(int32_t index) const;
    int16_t getShortLE(int32_t index) const;

    uint16_t getUnsignedShort(int32_t index) const;
    uint16_t getUnsignedShortLE(int32_t index) const;

    int32_t getInt(int32_t index) const;
    int32_t getIntLE(int32_t index) const;

    uint32_t getUnsignedInt(int32_t index) const;
    uint32_t getUnsignedIntLE(int32_t index) const;

    int64_t getLong(int32_t index) const;
    int64_t getLongLE(int32_t index) const;

    uint64_t getUnsignedLong(int32_t index) const;
    uint64_t getUnsignedLongLE(int32_t index) const;

    float getFloat(int32_t index) const;
    float getFloatLE(int32_t index) const;

    double getDouble(int32_t index) const;
    double getDoubleLE(int32_t index) const;

    /**
     * 以下方法分别从readerIndex位置读取8/16/32/64位的数据，readerIndex根据实际读出的位数增加
     */
    int8_t readChar();
    uint8_t readUnsignedChar();

    int16_t readShort();
    int16_t readShortLE();

    uint16_t readUnsignedShort();
    uint16_t readUnsignedShortLE();

    int32_t readInt();
    int32_t readIntLE();

    uint32_t readUnsignedInt();
    uint32_t readUnsignedIntLE();

    int64_t readLong();
    int64_t readLongLE();

    uint64_t readUnsignedLong();
    uint64_t readUnsignedLongLE();

    ByteBuf readBytes(int32_t length);

    double readDouble();
    double readDoubleLE();

    /**
     * readerIndex增加指定长度
     */
    ByteBuf &skipBytes(int32_t length);

    /**
     * 以下方法在index位置写入8/16/32/64位的数据，如果index超出现有数据范围，则追加，否则插入，不会修改readerIndex和writerIndex
     */
    ByteBuf &setChar(int32_t index, int8_t value);
    ByteBuf &setUnsignedChar(int32_t index, uint8_t value);

    ByteBuf &setShort(int32_t index, int16_t value);
    ByteBuf &setShortLE(int32_t index, int16_t value);

    ByteBuf &setUnsignedShort(int32_t index, uint16_t value);
    ByteBuf &setUnsignedShortLE(int32_t index, uint16_t value);

    ByteBuf &setInt(int32_t index, int32_t value);
    ByteBuf &setIntLE(int32_t index, int32_t value);

    ByteBuf &setUnsignedInt(int32_t index, uint32_t value);
    ByteBuf &setUnsignedIntLE(int32_t index, uint32_t value);

    ByteBuf &setLong(int32_t index, int64_t value);
    ByteBuf &setLongLE(int32_t index, int64_t value);

    ByteBuf &setUnsignedLong(int32_t index, uint64_t value);
    ByteBuf &setUnsignedLongLE(int32_t index, uint64_t value);

    ByteBuf &setBytes(int32_t index, const ByteBuf &buf);
    ByteBuf &setBytes(int32_t index, const char *data, int32_t size);

    /**
     * 从index位置填充指定长度的0，index超出现有数据，则追加，否则，插入，readerIndex和writerIndex不会改变
     */
    ByteBuf &setZero(int32_t index, int32_t length);

    /**
     * 以下方法会在writerIndex位置写入8/16/32/64位数据，若writerIndex超出现有数据范围，则追加并调整，否则插入，writerIndex根据实际写入的情况调整位置
     */
    ByteBuf &writeChar(int8_t value);

    ByteBuf &writeUnsignedChar(uint8_t value);

    ByteBuf &writeShort(int16_t value);
    ByteBuf &writeShortLE(int16_t value);

    ByteBuf &writeUnsignedShort(uint16_t value);
    ByteBuf &writeUnsignedShortLE(uint16_t value);

    ByteBuf &writeInt(int32_t value);
    ByteBuf &writeIntLE(int32_t value);

    ByteBuf &writeUnsignedInt(uint32_t value);
    ByteBuf &writeUnsignedIntLE(uint32_t value);

    ByteBuf &writeLong(int64_t value);
    ByteBuf &writeLongLE(int64_t value);

    ByteBuf &writeUnsignedLong(uint64_t value);
    ByteBuf &writeUnsignedLongLE(uint64_t value);

    ByteBuf &writeBytes(const ByteBuf &buf);
    ByteBuf &writeBytes(const char *data, int32_t size);

    /**
     * 从writerIndex位置填充指定长度的0，writerIndex超出现有数据，则追加并调整，否则插入，writerIndex根据实际写入的情况调整位置
     */
    ByteBuf &writeZero(int32_t length);

private:
    std::unique_ptr<ByteBufData> d;
};
}

#endif//SH_BYTE_BUF_H