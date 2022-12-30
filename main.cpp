#include <iostream>
#include <list>
#include <queue>
#include "base64.h"
#include "argus.h"
#include "defs.h"
#include "ladon.h"
#include "aes.hpp"
#include "hexdump.hpp"
extern "C"
#include "crypto/pkcs7_padding.h"


int show_size() {
    printf("char %lu\n", sizeof(char));
    printf("short %lu\n", sizeof(short));
    printf("int %lu\n", sizeof(int));
    printf("long %lu\n", sizeof(long));
    printf("long long %lu\n", sizeof(long long));
    printf("uint32_t %lu\n", sizeof(uint32_t));
    printf("uint64_t %lu\n", sizeof(uint64_t));
    return 0;
}


int gen_table(const uint64_t args[4]) {
    uint64_t array[72 + 4] = {0};
    for (int i = 0; i < 4; ++i) {
        array[i] = args[i];
    }
    for (int i = 0; i < 72; ++i) {
        uint64_t x12 = ((0x3DC94C3A046D678BuLL >> (i % 0x3Eu)) & 1 | 0xFFFFFFFFFFFFFFFCLL);
        uint64_t x9 = array[i + 1] ^ __ROR__(array[i + 3], 3);
        uint64_t x11 = array[i + 0] ^ x12;
        x11 = x11 ^ x9;
        uint64_t x10 = (x9 << 0x3f);
        uint64_t x8 = x10 | (x9 >> 1);
        x8 = x11 ^ x8;
        array[i + 4] = x8;
        printf("%llx\n", x8);
    }
    return 0;
}


// 生成出来是等价的
int test_encode() {
//    uint64_t array[] = {
//            0xd743168ff0c465a4,
//            0xdb8a0e364afced16,
//            0x6d1c003a02af01b1,
//            0x69fc8b81cba594f5,
//    };
//    gen_table(array);

//    uint64_t key[4] = {0xd743168ff0c465a4, 0xdb8a0e364afced16, 0x6d1c003a02af01b1, 0x69fc8b81cba594f5};
//    uint64_t ct[2] = {0, 0};
//    uint64_t pt[2] = {0x0210048280a4d208, 0x04220db8da95ce18};
//    uint64_t pt_1[2] = {0};
//    uint64_t ct_1[2] = {0x3bf72a87efe7b868, 0x8d2b5579afc8a3a0};
//
//    simon_enc(pt, ct, key);
//    printf("Key:          %016llx %016llx %016llx %016llx\n", key[3], key[2], key[1], key[0]);
//    printf("Plaint text:  %016llx %016llx\n", pt[1], pt[0]);
//    printf("Cipher text:  %016llx %016llx\n", ct[1], ct[0]);
    return 0;
}

// 得到异或的数，和手机生成的完全一样。
int get_random() {
    // 0x29955356L FF FB 57 47
    // 0x11223344L FF FE EF CD
    uint32_t random_num = 0x11223344L;
    uint32_t w8 = random_num >> 0x10;

    uint32_t second = ((uint8_t *)&w8)[0]; // 22
    printf("second: %x\n", second);
    uint32_t first = ((uint8_t *)&w8)[1];  // 11
    printf("first: %x\n", first);

    uint32_t x8 = second << 0xb;
    printf("%x\n", x8);

    x8 = x8 | first;
    printf("%x\n", x8);

    uint32_t v = (second >> 5);
    x8 = x8 ^ v; // 0x1不对
    x8 = x8 ^ second;
    printf("%x\n", x8);
    x8 = ~x8;
    printf("%x\n", x8);

    return 0;
}

int test_ladon() {
    std::string ladon = make_ladon(1670385975);
    std::cout << "ladon " << ladon << std::endl;
    return 0;
}

int test_argus() {
    const char *argus[] = {
//            "wni3DJwEiI+HxHUBV7pRPrlbBVHGWB0RLrn7nDZpkgT/7b7yUnNoVRYNacd+enlrqS/1/SJZ5peIWg8VIVaXj0mgYHLjK0rGwozg+11YG1QIFo8ABvRjVgo19d2zk2s8Kl8Tuvw5xpScKcTRHW9KpX5IiCTxo/YVdDXipJFgiI/41XgVLQ6Xeb6idXWLtCyt7k/Qt54B50bT1hQ5VcVy0pZufF/4/4Ehd1+L7sHBBHQQsA==",
//            "VlMKbTQsAIy4ORbvTJviXPX7vhiGcmQPBKnlyYShf0cwJ4voq++9uwan82uPkQ/I6NhvUA5jCq5QVddN0gEIcTIpKicQVVTHQVwDv33Inn1RFCpK3/DA8TRhaeF4hFSrtlFMb4DFMOqA976S6yrPxgeSdWr9DI9EwYhbyNSGX9RHRW5YKSckBS0JrDK/B2Iv2R4WcIt5WkVwu8a+3WmQHaxXMXvpzAZiVtzuXxI8SNJZ6gIk62MIXOOIZEA3BHxFUQJNyIuG5UETTukp2HieOFc8",
//            "cQKgXKGLXaNm/ndNh695cdi3aUj4rh9roQZXJw6iNO1RD7//sNmsFNCOgiGPwu2qMxDbPvb7NNnrQbEah2KUY2O+wGivO6C32EIvkyY1WR0BfCv4KEMukqyK+ieWcltN5P2+D+k/nq8tAkSLDObOIcCxCti9NfATJUeezKfxNF4ws0cKn0RRtjjNfEaeOg33Cdze/3SeVhS4cWSyb31PYpdzMh5ldkUd55VFhbDSGRuboL75Q+SDOprXeqqIkYXMLo3W/TASHS1gBkuM1UUcJBka",
            "+FT5llXVSaCkCWpbSkokeJ77sKJDGjWb8jH6nvzUot7LDDbLJWWTvZHju4fAqI2iaO/yKmfwXGgnJIQIU4yzkbSPsqOwdMXPkq67KJoLuWAcI4ZcU158OuHSG8aBkrk5uJ9wmhNpSX5wl9sUO5/+TC2kwn10F4LohAO/vCCrzApbTkrkEmSBMLloTtugU6lXOpBOwyGri8Q0o1ZGmgtd2r26qo0i24cPW+1WnbyvyMDB7kgn5oNx+4jsj1EzUr+ONjVcwFJeDnuFpiSEfqk5+2v6",
    };
    for (int i = 0; i < sizeof(argus) / sizeof (argus[0]); ++i) {
        decrypt_argus(argus[i]);
        printf("\n\n\n\n");
    }
    return 0;
}


void test_aes_cbc_decrypt() {

//    uint8_t data[] = {
//            0x5d,0x87,0x43,0xe5,0xc6,0x68,0x3b,0x38,0x6a,0x40,0xd8,0x1e,0x72,0xac,0x50,0x7b
//    };
//    const char *key_iv = "0000000000000000";
//    uint8_t *key = (uint8_t*)key_iv;
//    uint8_t *iv = (uint8_t*)key_iv;
//    AES_ctx ctx;
//    AES_init_ctx_iv(&ctx, key, iv);
//    AES_CBC_decrypt_buffer(&ctx, data, 16);


    uint8_t buffer[32] = {0x31};


    std::cout << Hexdump(buffer, sizeof buffer) << std::endl;

    pkcs7_padding_pad_buffer(buffer, 1, 15, 16);


    std::cout << Hexdump(buffer, sizeof buffer) << std::endl;


}



int main() {
    get_random();
//    test_argus();

    test_aes_cbc_decrypt();

    unsigned char protobuf[] = {
            0x08, 0xD2, 0xA4, 0x80, 0x82, 0x04, 0x10, 0x02, 0x18, 0xC4, 0x88, 0x89, 0x91, 0x02, 0x22, 0x04,
            0x33, 0x30, 0x31, 0x39, 0x32, 0x0A, 0x31, 0x36, 0x31, 0x31, 0x39, 0x32, 0x31, 0x37, 0x36, 0x34,
            0x3A, 0x06, 0x32, 0x33, 0x2E, 0x32, 0x2E, 0x30, 0x42, 0x14, 0x76, 0x30, 0x34, 0x2E, 0x30, 0x34,
            0x2E, 0x30, 0x30, 0x2D, 0x6D, 0x6C, 0x2D, 0x61, 0x6E, 0x64, 0x72, 0x6F, 0x69, 0x64, 0x48, 0x80,
            0x80, 0xA0, 0x40, 0x52, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xBE, 0xCB,
            0x87, 0x8F, 0xCC, 0x8D, 0xA8, 0x02, 0x6A, 0x06, 0xC2, 0xD4, 0xA2, 0x2F, 0x7C, 0x3A, 0x72, 0x06,
            0x70, 0xC4, 0x0E, 0x4F, 0xB2, 0x18, 0x7A, 0x0E, 0x08, 0x02, 0x10, 0xBE, 0xE1, 0x54, 0x18, 0xBE,
            0xE1, 0x54, 0x20, 0xBE, 0xE1, 0x54, 0xA2, 0x01, 0x04, 0x6E, 0x6F, 0x6E, 0x65, 0xA8, 0x01, 0xE2,
            0x05
    };
    encrypt_argus(protobuf, sizeof(protobuf));

    return 0;
}
