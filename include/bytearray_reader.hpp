#pragma once

// ba
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
 * read operations with vector like with byte array.
 * It holds const reference to std::vector with 1 byte width data.
 * (std::byte, uint8_t or unsigned char)
 */
template <typename ValueType, typename Allocator>
class bytearray_reader {
public:
    using vector = std::vector<ValueType, Allocator>;

private:
    /**
     * @brief Stream output function.
     */
    friend std::ostream& operator<<(std::ostream& ostream, const bytearray_reader& arr) {
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
    explicit bytearray_reader(const vector& container)
        : m_container(container) {}

    /**
     * @brief Method for getting constant reference to internal container.
     * @return Constant reference to internal container.
     */
    const vector& container() const { return m_container; }

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

    const vector& m_container;
};
}  // namespace ba

// Allowed
namespace std {
template <typename ValueType, typename Allocator>
std::string to_string(const ba::bytearray_reader<ValueType, Allocator>& processor) {
    std::stringstream ss;

    for (auto&& b : processor.container()) {
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(b);
    }

    return ss.str();
}
}  // namespace std
