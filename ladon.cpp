#include <iostream>
#include <queue>
#include "base64.h"
#include "hexdump.hpp"
#include "defs.h"

extern "C" {
#include "md5.h"
}

template<class T> T get_type_data(const uint8_t *ptr, int index) {
    int offset = sizeof(T) * index;
    return *(T *)(ptr + offset);
}

template<class T> void set_type_data(const uint8_t *ptr, int index, T data) {
    int offset = sizeof(T) * index;
    *(T *)(ptr + offset) = data;
}


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


int encrypt_ladon_input(uint8_t *hash_table, uint8_t input[16], uint8_t output[16]) {
    auto data0 = get_type_data<uint64_t>(input, 0);
    auto data1 = get_type_data<uint64_t>(input, 1);
    for (int i = 0; i < 0x22; ++i) {
        auto hash = get_type_data<uint64_t>(hash_table, i);
        data1 = hash ^ (data0 + __ROR__(data1, 8));
        data0 = data1 ^ __ROR__(data0, 0x3d);
    }
    set_type_data(output, 0, data0);
    set_type_data(output, 1, data1);
    return 0;
}

int encrypt_ladon(const char *md5hex, uint8_t *data, uint32_t size, uint8_t *output) {
    uint8_t hash_table[272 + 16] = {0};
    memcpy(hash_table, md5hex, 32);

    std::queue<uint64_t> temp;
    for (int i = 0; i < 4; ++i) {
        temp.push(get_type_data<uint64_t>(hash_table, i));
    }

    uint64_t buffer_b0;
    uint64_t buffer_b8;

    buffer_b0 = temp.front();
    temp.pop();
    buffer_b8 = temp.front();
    temp.pop();

    for (int i = 0; i < 0x22; ++i) {
        uint64_t x9 = buffer_b0;
        uint64_t x8 = buffer_b8;

//        printf("x9=%llx x8=%llx\n", x9, x8);

        x8 = __ROR__(x8, 8);
        x8 = x8 + x9;
        x8 = x8 ^ i;

        // save x8
        temp.push(x8);

        x8 = x8 ^ __ROR__(x9, 61);

//        printf("output = %llx\n", x8);

        set_type_data(hash_table, i + 1, x8);

        buffer_b0 = x8;
        buffer_b8 = temp.front();
        temp.pop();
    }

    std::cout << Hexdump(hash_table, 272 + 16) << std::endl;

    uint32_t newSize = ((size / 16) + ((size % 16) > 0 ? 1 : 0)) * 16;
    auto *newInput = new uint8_t[newSize];
    memset(newInput, 0x06, newSize); // 0x06填充
    memcpy(newInput, data, size);

    for (int i = 0; i < newSize / 16; ++i) {
        encrypt_ladon_input(hash_table, &newInput[i * 16], &output[i * 16]);
    }
    std::cout << Hexdump(output, newSize) << std::endl;
    return 0;
}

std::string make_ladon(uint32_t khronos) {
    uint32_t random_num = 0x4ec5e0ea;
    char data[32] = {0};
    std::snprintf(data, sizeof data, "%u-1588093228-1128", khronos);

    uint8_t keygen[8] = {'\0', '\0', '\0', '\0', '1', '1', '2', '8'};
    memcpy(keygen, &random_num, sizeof random_num);
    auto md5hex = md5bytes(keygen, 8);

    uint32_t size = strlen(data);
    uint32_t newSize = ((size / 16) + ((size % 16) > 0 ? 1 : 0)) * 16;

    // 扩展加随机数据
    auto *output = new uint8_t[newSize + 4];
    memcpy(output, &random_num, sizeof random_num);
    encrypt_ladon(md5hex.c_str(), (uint8_t *) data, size, &output[4]);

    std::string result = base64_encode(output, newSize + 4);

    delete[] output;
    return result;
}
