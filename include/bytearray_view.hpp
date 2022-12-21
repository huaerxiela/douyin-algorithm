#pragma once

#include <bytearray.hpp>

namespace ba {
/**
 * @brief Class, that describes
 * bytearray representation with different size
 * based values.
 * @tparam Allocator Allocator.
 */
template <typename ValueType, typename Allocator = std::allocator<ValueType> >
class bytearray_view {
    using container = bytearray_processor<ValueType, Allocator>;

    /**
     * @brief Stream output function.
     */
    friend std::ostream& operator<<(std::ostream& ostream, const bytearray_view& container) {
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
                << "}, Length: " << std::dec << container.size() << ')' << std::endl;

        ostream.flags(oldFlags);
        ostream.precision(oldPrec);
        ostream.fill(oldFill);

        return ostream;
    }

public:
    using size_type = typename container::size_type;
    using value_type = typename container::value_type;

    /**
     * @brief Constructor.
     * @param bytearray Base byte array.
     */
    explicit bytearray_view(container& bytearray)
        : m_byteArray(bytearray)
        , m_start(0)
        , m_size(m_byteArray.container().size()) {}

    /**
     * @brief Constructor with boundaries.
     * @param bytearray Initial byte array.
     * @param start Start position.
     * @param size Size.
     */
    bytearray_view(container& bytearray, size_type start, size_type size)
        : m_byteArray(bytearray)
        , m_start(start)
        , m_size(size) {
        assert(start <= m_byteArray.container().size());
        assert(start + size <= m_byteArray.container().size());
    }

    /**
     * @brief Method for getting initial
     * byte array object.
     */
    container& bytearray() { return m_byteArray; }

    /**
     * @brief Method for getting constant reference
     * to initial byte array object.
     */
    const container& bytearray() const { return m_byteArray; }

    /**
     * @brief Method for getting begin of this
     * view.
     */
    typename container::vector::iterator begin() { return m_byteArray.container().begin() + m_start; }

    /**
     * @brief Method for getting begin of this
     * view.
     */
    typename container::vector::const_iterator begin() const { return cbegin(); }

    /**
     * @brief Method for getting end of this view.
     */
    typename container::vector::iterator end() { return m_byteArray.container().begin() + (m_start + m_size); }

    /**
     * @brief Method for getting end of this view.
     */
    typename container::vector::const_iterator end() const { return cend(); }

    /**
     * @brief Method for getting constant begin of this
     * view.
     */
    typename container::vector::const_iterator cbegin() const { return m_byteArray.container().cbegin() + m_start; }

    /**
     * @brief Method for getting constant end of this
     * view.
     */
    typename container::vector::const_iterator cend() const { return m_byteArray.container().cbegin() + (m_start + m_size); }

    /**
     * @brief Method for getting reverse begin of this
     * view.
     */
    typename container::vector::reverse_iterator rbegin() {
        return m_byteArray.container().rbegin() + (m_byteArray.container().size() - (m_start + m_size));
    }

    /**
     * @brief Method for getting reverse end of this view.
     */
    typename container::vector::reverse_iterator rend() {
        return m_byteArray.container().rbegin() + (m_byteArray.container().size() - m_start);
    }

    /**
     * @brief Method for getting constant reverse begin of this view.
     */
    typename container::vector::const_reverse_iterator crbegin() const {
        return m_byteArray.container().crbegin() + (m_byteArray.container().size() - (m_start + m_size));
    }

    /**
     * @brief Method for getting constant reverse end of this view.
     */
    typename container::vector::const_reverse_iterator crend() const {
        return m_byteArray.container().crbegin() + (m_byteArray.container().size() - m_start);
    }

    /**
     * @brief Method for getting view size.
     * @return Size in bytes.
     */
    typename container::size_type size() const { return m_size; }

    /**
     * @brief Method for access to specified element.
     * @param i Index.
     * @return Reference to value.
     */
    typename container::value_type& operator[](size_type i) { return m_byteArray.container()[m_start + i]; }

    /**
     * @brief Method for const access to specified element.
     * @param i Index.
     */
    typename container::value_type operator[](size_type i) const { return m_byteArray.container()[m_start + i]; }

    /**
     * @brief Method for access to specified element with
     * bounds checking.
     * @param i Index.
     * @return Reference to value.
     */
    typename container::value_type& at(size_type i) { return m_byteArray.container().at(i); }

    /**
     * @brief Method for const access to specified element with
     * bounds checking.
     * @param i Index.
     * @return Reference to value.
     */
    typename container::value_type at(size_type i) const { return m_byteArray.container().at(i); }

    /**
     * @brief Method, that returns maximum number of
     * elements.
     */
    typename container::size_type max_size() const { return std::numeric_limits<typename container::size_type>::max(); }

    /**
     * @brief Method for pushing back some trivially copyable type
     * with defined endianness.
     * @tparam T Type.
     * @param value Value.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back(T value, endianness order = endianness::big) {
        m_byteArray.template insert<T>(m_start + m_size, value, order);

        m_size += sizeof(T);
    }

    /**
     * @brief Method for pushing back some part of trivially copyable type
     * value with defined endianness.
     * @tparam T Type.
     * @param value Value.
     * @param size Amount of bytes.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back_part(T value,
                                                                                       size_type size,
                                                                                       endianness order = endianness::big) {
        assert(sizeof(T) >= size && "Can't push size bigger, than type.");

        m_byteArray.template insert_part<T>(m_start + m_size, value, size, order);

        m_size += size;
    }

    /**
     * @brief Method for pushing back multiple instances of some trivially
     * copyable type value with defined endiannes for each element.
     * @tparam T Type.
     * @param value Value.
     * @param amount Amount of elements.
     * @param order Endianness.
     */
    template <typename T>
    typename std::enable_if<std::is_trivially_copyable<T>::value>::type push_back_multiple(T value,
                                                                                           size_type amount,
                                                                                           endianness order = endianness::big) {
        m_byteArray.template insert_multiple<T>(m_start + m_size, value, amount, order);

        m_size += amount * sizeof(T);
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
        assert(position <= m_size && "Position is out of bounds.");

        m_byteArray.template insert<T>(m_start + position, value, order);

        m_size += sizeof(T);
    }

    /**
     * @brief Method for insertion part of some trivially
     * copyable value with defined endianness.
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
        assert(position <= m_size && "Position is out of bounds.");

        m_byteArray.template insert_part<T>(m_start + position, value, size, order);

        m_size += size;
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
        assert(position <= m_size && "Position is out of bounds.");

        m_byteArray.template insert_multiple<T>(m_start + position, value, amount, order);

        m_size += sizeof(T) * amount;
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
        m_byteArray.template insert_multiple(m_start + position, begin, last, order);

        m_size += sizeof(typename std::iterator_traits<InputIterator>::value_type) * std::distance(begin, last);
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
        assert(position + sizeof(T) <= size() && "Position + type size is out of bounds.");

        m_byteArray.template set<T>(m_start + position, value, order);
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
        assert(position + sizeof(T) <= size() && "Position + type size if out of bounde.");

        return m_byteArray.template read<T>(m_start + position, order);
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
        assert(position + size <= this->size() && "Position + size is out of bounds.");

        return m_byteArray.template read_part<T>(m_start + position, size, order);
    }

    /**
     * @brief Method for checking whether the view is empty.
     */
    bool empty() const { return m_size == 0; }

    const ValueType* data() const { return m_byteArray.container().data() + m_start; }

    ValueType* data() { return m_byteArray.container().data() + m_start; }

    /**
     * @brief Method for comparison of different views.
     */
    template <typename RhsAllocator>
    bool operator==(const bytearray_view<value_type, RhsAllocator>& rhs) {
        if (size() != rhs.size()) {
            return false;
        }

        for (size_type i = 0; i < size(); ++i) {
            if (operator[](i) != rhs[i]) {
                return false;
            }
        }

        return true;
    }

private:
    container& m_byteArray;
    size_type m_start;
    size_type m_size;
};
}  // namespace ba

// Allowed
namespace std {
template <typename ValueType, typename Allocator>
std::string to_string(const ba::bytearray_view<ValueType, Allocator>& processor) {
    std::stringstream ss;

    for (auto&& b : processor) {
        ss << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << int(b);
    }

    return ss.str();
}
}  // namespace std