#pragma once


extern "C" {
#include "pkcs7_padding.h"
}

class ByteBuf {
private:
    uint8_t *mem = nullptr;
    uint32_t data_size = 0;
    uint32_t pos = 0;

    ByteBuf() = default;
public:
    explicit ByteBuf(uint32_t size) {
        this->data_size = size;
        this->mem = new uint8_t[size];
    }
    ByteBuf(const uint8_t *data, uint32_t size) {
        this->data_size = size;
        this->mem = new uint8_t[size];
        memcpy(this->mem, data, size);
    }

    ~ByteBuf() {
        if (this->mem != nullptr) {
            delete[] this->mem;
            this->mem = nullptr;
            this->data_size = 0;
        }
    }

    uint8_t * data() {
        return this->mem;
    }

    uint32_t read_uint32() {
        if (this->mem == nullptr) {
            return -1;
        }
        if (pos + 4 > data_size) {
            return -1;
        }
        uint32_t result = *(uint32_t *)&mem[pos];
        pos = 4;
        return result;
    }

    [[nodiscard]] uint32_t size() const {
        return data_size;
    }

    uint32_t remove_padding() {
        auto padding_size = pkcs7_padding_data_length(mem, data_size, 16);
        if (padding_size == 0) {
            return data_size;
        }
        data_size = padding_size;
        auto dst = new uint8_t[data_size];
        std::memcpy(dst, mem, data_size);
        delete[] mem;
        mem = dst;
        return data_size;
    }
};