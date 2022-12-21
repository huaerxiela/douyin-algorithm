#pragma once

#include "bytearray_processor.hpp"

namespace ba {
/**
 * @brief Class, that implements byte array, that
 * holds std::vector object and has bytearray_processor
 * interface.
 * @tparam Allocator Allocator for std::vector.
 */
template <typename Allocator = std::allocator<std::byte>>
class bytearray : public bytearray_processor<std::byte, Allocator>, private std::vector<std::byte, Allocator> {
    using vector = std::vector<std::byte, Allocator>;
    using processor = bytearray_processor<std::byte, Allocator>;

public:
    using typename processor::size_type;
    using typename processor::value_type;
    using typename vector::const_iterator;
    using typename vector::const_reverse_iterator;
    using typename vector::iterator;
    using typename vector::reverse_iterator;

    /**
     * @brief Default constructor.
     */
    bytearray()
        : processor(*static_cast<vector*>(this))
        , vector() {}

    /**
     * @brief Move constructor.
     */
    bytearray(bytearray<Allocator>&& rhs) noexcept
        : processor(*static_cast<vector*>(this))
        , vector(std::move(rhs.container())) {}

    /**
     * @brief Default copy constructor.
     */
    bytearray(const bytearray<Allocator>& rhs)
        : processor(*static_cast<vector*>(this))
        , vector(rhs.container()) {}

    /**
     * @brief Constructor with initial size.
     */
    explicit bytearray(size_type sizeValue)
        : processor(*static_cast<vector*>(this))
        , vector(sizeValue) {}

    /**
     * @brief Initializer constructor.
     */
    bytearray(const value_type* val, size_type amount)
        : processor(*static_cast<vector*>(this))
        , vector(val, val + amount) {}

    /**
     * @brief Copy operator.
     */
    bytearray<Allocator>& operator=(const bytearray<Allocator>& rhs) {
        vector::operator=(rhs.container());

        return (*this);
    }

    /**
     * @brief Move operator.
     */
    bytearray<Allocator>& operator=(bytearray<Allocator>&& rhs) noexcept {
        vector::operator=(std::move(rhs.container()));

        return (*this);
    }

    using vector::operator[];
    using processor::insert;
    using processor::push_back;
    using vector::at;
    using vector::begin;
    using vector::capacity;
    using vector::cbegin;
    using vector::cend;
    using vector::clear;
    using vector::crbegin;
    using vector::crend;
    using vector::empty;
    using vector::end;
    using vector::rbegin;
    using vector::rend;
    using vector::reserve;
    using vector::size;
};
}  // namespace ba


static inline ba::bytearray<> operator"" _ba(const char* str, std::size_t len) {
    ba::bytearray array{};

    array.load_from_hex(std::string_view{str, len});

    return array;
}