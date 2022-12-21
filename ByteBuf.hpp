#pragma once


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
        if ((data_size % 16) != 0) {
            return 0;
        }
        uint8_t *end_pos = &mem[data_size];
        uint8_t byte = *(end_pos-1);
        for (int i = 0; i < 16; ++i) {
            uint8_t end = *(--end_pos);
            if (end != byte) {
                end_pos++;
                break;
            }
        }
        data_size = end_pos-mem;

        auto dst = new uint8_t[data_size];
        std::memcpy(dst, mem, data_size);
        delete[] mem;
        mem = dst;
        return data_size;
    }
};