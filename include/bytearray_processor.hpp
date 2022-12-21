#pragma once

// ba
#include <bytearray_reader.hpp>
#include <endianness.hpp>

// C++ STL
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>
#include <vector>

namespace ba {

/**
 * @brief Class, that implements
 * operations with vector like with byte array.
 * It holds reference to std::vector with 1 byte width data.
 * (std::byte, uint8_t or unsigned char)
 */
template <typename ValueType, typename Allocator>
class bytearray_processor : public bytearray_reader<ValueType, Allocator> {
public:
    using vector = std::vector<ValueType, Allocator>;

private:
    /**
     * @brief Stream output function.
     */
    friend std::ostream& operator<<(std::ostream& ostream, const bytearray_processor& arr) {
        // Printing header
        ostream << "ByteArray({" << std::endl;
        ostream << "               #-------------#-------------#-------------#-------------#" << std::endl;
        ostream << "               | 00 01 02 03 | 04 05 06 07 | 08 09 0A 0B | 0C 0D 0E 0F |" << std::endl;
        ostream << "               #-------------#-------------#-------------#-------------#";

        // Saving states
        auto oldFlags = ostream.flags();
        auto oldPrec = ostream.precision();
        auto oldFill = ostream.fill();

        // Changing fill character
        ostream.fill('0');

        const std::vector<ValueType>& container = arr.container();

        //
        size_type index = 0;
        for (index = 0; index < container.size() + (16 - (container.size() % 16)); ++index) {
            if (!(index % 16)) {
                if (index) {
                    ostream << "| ";
                }

                for (std::size_t asc = index - 16; asc < index; ++asc) {
                    if (container[asc] >= ValueType(' ') && container[asc] <= ValueType('~')) {
                        ostream << static_cast<char>(container[asc]);
                    } else {
                        ostream << '.';
                    }
                }

                ostream << std::endl << "    0x";
                ostream.width(8);
                ostream << std::hex << index << ' ';
            }

            if (!(index % 4)) {
                ostream << "| ";
            }

            if (index < container.size()) {
                ostream.width(2);
                ostream << std::uppercase << std::hex << static_cast<int>(container[index]) << ' ';
            } else {
                ostream << "   ";
            }
        }

        if (index) {
            ostream << "| ";
        }

        for (size_type asc = index - 16; asc < index; ++asc) {
            if (asc < container.size()) {
                if (container[asc] >= ValueType(' ') && container[asc] <= ValueType('~')) {
                    ostream << static_cast<char>(container[asc]);
                } else {
                    ostream << '.';
                }
            } else {
                ostream << ' ';
            }
        }

        ostream << std::endl
                << std::nouppercase << "               #-------------#-------------#-------------#-------------#" << std::endl
                << "}, Length: " << std::dec << container.size() << ", Capacity: " << std::dec << container.capacity() << ')' << std::endl;

        ostream.flags(oldFlags);
        ostream.precision(oldPrec);
        ostream.fill(oldFill);

        return ostream;
    }

public:
    static_assert(sizeof(ValueType) == 1, "Size of container element has to be 1 byte");

    using size_type = typename vector::size_type;
    using value_type = ValueType;

    /**
     * @brief Constructor. Accepts container reference to
     * operate with.
     * @param container Vector with type.
     */
    explicit bytearray_processor(vector& container)
        : bytearray_reader<ValueType, Allocator>(container)
        , m_container(container) {}

    /**
     * @brief Method for getting reference to internal container.
     * @return Reference to internal container.
     */
    vector& container() { return m_container; }

    /**
     * @brief Method for getting constant reference to internal container.
     * @return Constant reference to internal container.
     */
    const vector& container() const { return m_container; }

    /**
     * @brief Method for loading bytearray data from hex.
     * If bytearray already contains any data - it will be erased.
     * @param s Hex string. Example: "aabbccdd" or "AABBDDCC" or "AA BB CC DD"
     * @return Success. If fail - previous data will not be erased.
     */
    bool load_from_hex(const std::string& s) { return load_from_hex(std::string_view(s)); }

    /**
     * @brief Method for loading bytearray data from hex.
     * If bytearray already contains any data - it will be erased.
     * @param sv Hex string view. Example: "aabbccdd" or "AABBDDCC" or "AA BB CC DD"
     * @return Success. If fail - previous data will not be erased.
     */
    bool load_from_hex(const std::string_view& sv) {
        static auto validator = [](char c) { return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (c >= '0' && c <= '9'); };

        static auto char2int = [](char symbol) -> uint8_t {
            if (symbol >= '0' && symbol <= '9') {
                return static_cast<uint8_t>(symbol - '0');
            } else if (symbol >= 'A' && symbol <= 'F') {
                return static_cast<uint8_t>(symbol - 'A' + 10);
            } else if (symbol >= 'a' && symbol <= 'f') {
                return static_cast<uint8_t>(symbol - 'a' + 10);
            }

            throw std::runtime_error("Received unknown symbol");
        };

        // Checking string
        auto found = std::find_if(sv.begin(), sv.end(), [](char c) {
            return (c < 'a' || c > 'f') && (c < 'A' || c > 'F') && (c < '0' || c > '9') && (c != ' ');
        });

        if (found != sv.end()) {
            return false;
        }

        // Parsing
        auto count = std::count_if(sv.begin(), sv.end(), validator);

        m_container.clear();
        m_container.reserve(count);

        bool isFirst = true;
        uint8_t firstValue = 0;

        for (auto symbol : sv) {
            if (!validator(symbol)) {
                continue;
            }

            if (isFirst) {
                firstValue = char2int(symbol);

                isFirst = false;
            } else {
                m_container.push_back(std::byte(firstValue * 16 + char2int(symbol)));

                isFirst = true;
            }
        }

        return true;
    }

    /**
     * @brief Method for pushing back some trivially copyable type
     * with defined endianness.
     * @tparam T Type.
     * @param value Value.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back(T value, endianness order = endianness::big) {
        if (order == get_system_endianness()) {
            for (std::size_t i = 0; i < sizeof(T); ++i) {
                m_container.push_back(((ValueType*)(&value))[i]);
            }
        } else  // big
        {
            for (std::size_t i = sizeof(T); i > 0; --i) {
                m_container.push_back(((ValueType*)(&value))[i - 1]);
            }
        }
    }

    /**
     * @brief Method for pushing back some part of trivially copyable type
     * value with defined endianness.
     * @tparam T Type.
     * @param value Value.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back_part(T value,
                                                                                       size_type size,
                                                                                       endianness order = endianness::big) {
        assert(sizeof(T) >= size && "Can't push size bigger, than type.");

        if (order == get_system_endianness()) {
            for (std::size_t i = 0; i < size; ++i) {
                m_container.push_back(((ValueType*)(&value))[i]);
            }
        } else {
            for (std::size_t i = size; i > 0; --i) {
                m_container.push_back(((ValueType*)(&value))[i - 1]);
            }
        }
    }

    /**
     * @brief Method for pushing back several
     * copies of some trivially copyable value.
     * @tparam T Value type.
     * @param value Value.
     * @param amount Amount.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back_multiple(T value,
                                                                                           size_type amount,
                                                                                           endianness order = endianness::big) {
        for (size_type i = 0; i < amount; ++i) {
            push_back(value, order);
        }
    }

    /**
     * @brief Method for pushing back several
     * values from range.
     * @tparam InputIterator Input iterator type.
     * @param begin Begin iterator.
     * @param last Last iterator.
     * @param order Endianness.
     */
    template <typename InputIterator>
    typename std::enable_if<std::is_trivially_copyable<typename std::iterator_traits<InputIterator>::value_type>::value>::type
    push_back_multiple(InputIterator begin, InputIterator last, endianness order = endianness::big) {
        while (begin != last) {
            push_back(*begin, order);

            ++begin;
        }
    }

    /**
     * @brief Method for pushing back data from initializer list.
     * @tparam T Initializer list element type.
     * @param il Initializer list.
     * @param order Element pushing order.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back_multiple(std::initializer_list<T> il,
                                                                                           endianness order = endianness::big) {
        push_back_multiple(il.begin(), il.end(), order);
    }

    /**
     * @brief Method for insertion some trivially copyable type
     * with defined endianness.
     * @tparam T Type.
     * @param position Position.
     * @param value Value.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type insert(size_type position,
                                                                               T value,
                                                                               endianness order = endianness::big) {
        assert(position <= m_container.size() && "Position is out of bounds.");

        if (order == get_system_endianness()) {
            m_container.insert(m_container.begin() + position, ((ValueType*)&value), ((ValueType*)&value) + sizeof(value));
        } else {
            m_container.insert(m_container.begin() + position, std::reverse_iterator(((ValueType*)&value) + sizeof(value)),
                               std::reverse_iterator((ValueType*)&value));
        }
    }

    /**
     * @brief Method for insertion part of some trivially
     * copyable type with defined endianness.
     * @tparam T Type.
     * @param position Position.
     * @param value Value.
     * @param size Amount of bytes of value.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type insert_part(size_type position,
                                                                                    T value,
                                                                                    size_type size,
                                                                                    endianness order = endianness::big) {
        assert(sizeof(T) >= size && "Can't insert size bigger, than type.");

        if (order == get_system_endianness()) {
            m_container.insert(m_container.begin() + position, ((ValueType*)&value), ((ValueType*)&value) + size);
        } else {
            m_container.insert(m_container.begin() + position, std::reverse_iterator(((ValueType*)&value) + size),
                               std::reverse_iterator((ValueType*)&value));
        }
    }

    /**
     * @brief Method for insertion multiple same elements
     * with defined endianness.
     * @tparam T Type.
     * @param position Position.
     * @param value Value.
     * @param amount Amount of elements.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type insert_multiple(size_type position,
                                                                                        T value,
                                                                                        size_type amount,
                                                                                        endianness order = endianness::big) {
        // Moving everything
        m_container.insert(m_container.begin() + position, sizeof(T) * amount, ValueType(0x00));

        for (size_type i = 0; i < amount; ++i) {
            set<T>(position + sizeof(T) * i, value, order);
        }
    }

    /**
     * @brief Method for insertion multiple elements with
     * defined endianness.
     * @tparam InputIterator Iterator type.
     * @param position Position.
     * @param begin Begin iterator.
     * @param last Last iterator.
     * @param order Endianness.
     */
    template <typename InputIterator>
    typename std::enable_if<std::is_trivially_copyable<typename std::iterator_traits<InputIterator>::value_type>::value>::type
    insert_multiple(size_type position, InputIterator begin, InputIterator last, endianness order = endianness::big) {
        using val = typename std::iterator_traits<InputIterator>::value_type;

        auto count = std::distance(begin, last);

        // Moving everything
        m_container.insert(m_container.begin() + position, sizeof(val) * count, ValueType(0x00));

        size_type index = 0;

        while (begin != last) {
            set<val>(position + sizeof(val) * index, *begin, order);

            ++begin;
            ++index;
        }
    }

    /**
     * @brief Method for setting value in byte array
     * container.
     * @tparam T Value type.
     * @param position Set position.
     * @param value Value.
     * @param order Byte order.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type set(size_type position,
                                                                            T value,
                                                                            endianness order = endianness::big) {
        assert(position + sizeof(value) <= m_container.size() && "Position + type size is out of bounds.");

        if (order == get_system_endianness()) {
            for (std::size_t i = 0; i < sizeof(T); ++i) {
                m_container[position + i] = ((ValueType*)(&value))[i];
            }
        } else {
            for (std::size_t i = sizeof(T); i > 0; --i) {
                m_container[position + sizeof(T) - i] = ((ValueType*)(&value))[i - 1];
            }
        }
    }

    /**
     * @brief Method for performing translation of
     * byte array to some trivially copyable type.
     * @tparam T Type.
     * @param position Value position.
     * @param order Read order.
     * @return Read value.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value, T>::type read(size_type position,
                                                                                endianness order = endianness::big) const {
        assert(position + sizeof(T) <= m_container.size() && "Position + type size is out of bounds.");

        T value;

        if (order == get_system_endianness()) {
            for (std::size_t i = 0; i < sizeof(T); ++i) {
                ((ValueType*)(&value))[i] = m_container[position + i];
            }
        } else {
            for (std::size_t i = sizeof(T); i > 0; --i) {
                ((ValueType*)(&value))[i - 1] = m_container[position + sizeof(T) - i];
            }
        }

        return value;
    }

    /**
     * @brief Method for performing translation of
     * byte array to some part of trivially copyable type.
     * @tparam T Type.
     * @param position Value position.
     * @param order Read order.
     * @return Read value.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value, T>::type read_part(size_type position,
                                                                                     size_type size,
                                                                                     endianness order = endianness::big) const {
        assert(size <= sizeof(T) && "Size if bigger, than types size");
        assert(position + size <= m_container.size() && "Position + size is out of bounds.");

        T value;
        std::memset(&value, 0, sizeof(T));

        if (order == get_system_endianness()) {
            for (size_type i = 0; i < size; ++i) {
                ((ValueType*)(&value))[i] = m_container[position + i];
            }
        } else {
            for (size_type i = size; i > 0; --i) {
                ((ValueType*)(&value))[i - 1] = m_container[position + size - i];
            }
        }

        return value;
    }

private:
    /**
     * @brief Constexpr function for checking system endianness.
     * @return Endianness value.
     */
    [[nodiscard]] constexpr endianness get_system_endianness() const {
        union {
            uint32_t i;
            char c[4];
        } example = {0x01020304};

        return example.c[0] == 1 ? endianness::big : endianness::little;
    }

    vector& m_container;
};
}  // namespace ba

// Allowed
namespace std {
template <typename ValueType, typename Allocator>
std::string to_string(const ba::bytearray_processor<ValueType, Allocator>& processor) {
    std::stringstream ss;

    for (auto&& b : processor.container()) {
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(b);
    }

    return ss.str();
}
}  // namespace std
