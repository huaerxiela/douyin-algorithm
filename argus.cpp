//
// Created by jpacg on 2022/12/20.
//

#include "argus.h"
#include "base64.h"
#include "hexdump.hpp"
#include <string>
#include <iostream>
#include "common.h"
extern "C" {
#include "sm3.h"
#include "simon.h"
#include "pkcs7_padding.h"
}
#include "aes.hpp"

#include "bytearray.hpp"
#include "bytearray_processor.hpp"
#include "bytearray_reader.hpp"
#include "bytearray_view.hpp"
#include "ByteBuf.hpp"
#include "ByteBuf.h"


int encrypt_enc_pb(const uint8_t *encode_pb, uint32_t size, const uint8_t xor_array[8]) {
    auto *buffer = new uint8_t[size + 8];
    memset(buffer, 0, size + 8);
    memcpy(&buffer[8], encode_pb, size);
    std::reverse(buffer, buffer + size + 8);
    for (int i = 0; i < (size + 8); ++i) {
        buffer[i] = buffer[i] ^ xor_array[i % 4];
    }
//    std::cout << "加密后的pb:\n" << Hexdump(buffer, size+8) << std::endl;
    return 0;
}

int decrypt_enc_pb(uint8_t *data, uint32_t len) {
    // 后8位
    ByteBuf ba(&data[len-8], 8);

    for (int i = 0; i < len - 8; ++i) {
        uint8_t ch = data[i];
        data[i] = data[i] ^ ba.data()[i % 4];
//        printf("idx:%d mod:%02x     %02x ^ %02x = %02x\n", i, i % 8, ch, xor_array[i % 8], data[i]);
    }
    std::reverse(data, data+len);
    encrypt_enc_pb(&data[8], len-8, ba.data());
    return 0;
}

std::string aes_cbc_decrypt(uint8_t *data, uint32_t len, uint8_t key[16], uint8_t iv[16]) {
    std::string result;
    result.resize(padding_size(len));
    memcpy(result.data(), data, len);

    AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, (uint8_t *) result.data(), result.size());

    auto padding_size = pkcs7_padding_data_length(reinterpret_cast<uint8_t *>(result.data()), result.size(), 16);
    if (padding_size == 0) {
        return result;
    }
    return std::string(result.data(), padding_size);
}


int decrypt_argus(const char *x_argus) {
    auto argus = base64_decode(std::string(x_argus));
    uint16_t rand_right = *(uint16_t *)argus.data();

    auto sign_key = base64_decode(std::string("jr36OAbsxc7nlCPmAp7YJUC8Ihi7fq73HLaR96qKovU="));
//    auto sign_key = base64_decode(std::string("rBrarpWnr5SlEUqzs6l92ABQqgo5MUxAUoyuyVJWwow="));
    uint8_t aes_key[16] = {0};
    uint8_t aes_iv[16] = {0};
    md5(reinterpret_cast<uint8_t *>(sign_key.data()), 16, aes_key);
    md5(reinterpret_cast<uint8_t *>(sign_key.data() + 16), 16, aes_iv);

    std::string output = aes_cbc_decrypt(reinterpret_cast<uint8_t *>(argus.data() + 2), argus.size()-2, aes_key, aes_iv);

    std::cout << "aes result remove padding: \n" << Hexdump(output.data(), output.size()) << std::endl;
    // 第一个字节是0x35(手机)或者0xa6(unidbg), 不确定怎么来的, 步骤很多
    // 再后面四个字节是 random 的数据, 不知道干嘛用的
    // 再后面四个字节是 (01 02 0c 18) (01 d0 06 18) (01 d0 0f 18), 根据url query算来的，还没分析
    // 数据
    // 倒数2个字节是随机数高位

    uint32_t len = output.size();
    uint16_t rand_left = *(uint16_t *)&output.data()[len - 2];

    uint32_t random_num = rand_left;
    random_num = random_num << 16 | rand_right;
    printf("%x %x random=%x\n", rand_right, rand_left, random_num);

    uint32_t bsize = len-9-2;
    auto *b_buffer = new uint8_t[bsize];
    memcpy(b_buffer, &output.data()[9], bsize);

    std::cout << "enc_pb 解密前:\n" << Hexdump(b_buffer, bsize) << std::endl;
    decrypt_enc_pb(b_buffer, bsize);
    std::cout << "enc_pb 解密后(前8个字节是异或):\n" << Hexdump(b_buffer, bsize) << std::endl;

    // sm3(sign_key + random + sign_key)
    auto size = sign_key.size() + 4 + sign_key.size();
    sh::ByteBuf sm3Buf;
    sm3Buf.writeBytes(sign_key.data(), sign_key.size());
    sm3Buf.writeBytes(reinterpret_cast<const char *>(&random_num), 4);
    sm3Buf.writeBytes(sign_key.data(), sign_key.size());
    unsigned char sm3_output[32] = {0};
    sm3((unsigned char *) sm3Buf.data(), size, sm3_output);

    uint64_t key[] = {0, 0, 0, 0};
    memcpy(key, sm3_output, 32);

    uint64_t ct[2] = {0, 0};
    uint64_t pt[2] = {0, 0};

    uint8_t *p = &b_buffer[8];
    uint32_t new_len = bsize - 8;

    auto *protobuf = new uint8_t[new_len];
    for (int i = 0; i < new_len / 16; ++i) {
        memcpy(&ct, &p[i * 16], 16);
        simon_dec(pt, ct, key);
        // 定位到行，写一行(16字节)
        memcpy(&protobuf[i * 16], &pt[0], 8);
        memcpy(&protobuf[i * 16 + 8], &pt[1], 8);
    }

    ByteBuf pb_ba(protobuf, new_len);
    pb_ba.remove_padding();

    std::cout << "protobuf:\n" << Hexdump(pb_ba.data(), pb_ba.size()) << std::endl;
    return 0;
}


std::string encrypt_argus(const uint8_t *protobuf, uint32_t protobuf_size) {

    auto sign_key = base64_decode(std::string("jr36OAbsxc7nlCPmAp7YJUC8Ihi7fq73HLaR96qKovU="));
//    auto sign_key = base64_decode(std::string("rBrarpWnr5SlEUqzs6l92ABQqgo5MUxAUoyuyVJWwow="));
    uint8_t aes_key[16] = {0};
    uint8_t aes_iv[16] = {0};
    md5((uint8_t *)sign_key.data(), 16, aes_key);
    md5((uint8_t *)sign_key.data() + 16, 16, aes_iv);

    uint32_t random_num = 0x29955356;

    // sm3(sign_key + random + sign_key)
    auto size = sign_key.size() + 4 + sign_key.size();
    sh::ByteBuf sm3Buf;
    sm3Buf.writeBytes(sign_key.data(), sign_key.size());
    sm3Buf.writeBytes(reinterpret_cast<const char *>(&random_num), 4);
    sm3Buf.writeBytes(sign_key.data(), sign_key.size());
    unsigned char sm3_output[32] = {0};
    sm3((unsigned char *) sm3Buf.data(), size, sm3_output);

    uint64_t key[] = {0, 0, 0, 0};
    memcpy(key, sm3_output, 32);

    uint32_t buffer_size = padding_size(protobuf_size);
    ByteBuf byteBuf(buffer_size);
    memcpy(byteBuf.data(), protobuf, protobuf_size);

    pkcs7_padding_pad_buffer(byteBuf.data(), protobuf_size, buffer_size, 16);
    std::cout << "padding protobuf buffer\n" << Hexdump(byteBuf.data(), buffer_size) << std::endl;
    for (int i = 0; i < (buffer_size/16); ++i) {
        uint64_t *ptr = (uint64_t *)&byteBuf.data()[i * 16];
        uint64_t ct[2] = {0, 0}; // 解密填充这个
        uint64_t pt[2] = {ptr[0], ptr[1]}; // 加密填充这个
        simon_enc(pt, ct, key);
        memcpy(ptr, &ct, sizeof ct);
    }

    std::cout << "protobuf enc:\n" << Hexdump(byteBuf.data(), buffer_size) << std::endl;
    return "";
}