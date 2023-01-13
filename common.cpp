//
// Created by jpacg on 2023/1/13.
//

#include <cstdint>
#include <string>

extern "C" {
#include "crypto/md5.h"
}

uint32_t padding_size(uint32_t size) {
    uint32_t mod = size % 16;
    if (mod > 0) {
        return size + (16 - mod);
    }
    return size;
}

template<class T> T get_type_data(const uint8_t *ptr, int index) {
    int offset = sizeof(T) * index;
    return *(T *)(ptr + offset);
}

template<class T> void set_type_data(const uint8_t *ptr, int index, T data) {
    int offset = sizeof(T) * index;
    *(T *)(ptr + offset) = data;
}

template uint64_t get_type_data(const uint8_t *ptr, int index);
template void set_type_data(const uint8_t *ptr, int index, uint64_t data);

std::string bytes_to_hex_string(const uint8_t *data, uint32_t data_length) {
    char const hex_chars[16] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };
    std::string result;
    for( int i = 0; i < data_length; ++i ) {
        uint8_t byte = data[i];
        result += hex_chars[ ( byte & 0xF0 ) >> 4 ];
        result += hex_chars[ ( byte & 0x0F ) >> 0 ];
    }
    return result;
}

std::string md5bytes(uint8_t *data, uint32_t size) {
    MD5Context ctx;
    md5Init(&ctx);
    md5Update(&ctx, data, size);
    md5Finalize(&ctx);
    return bytes_to_hex_string(ctx.digest, 16);
}

int md5(uint8_t *data, uint32_t size, uint8_t result[16]) {
    MD5Context ctx;
    md5Init(&ctx);
    md5Update(&ctx, data, size);
    md5Finalize(&ctx);
    memcpy(result, ctx.digest, 16);
    return 0;
}
