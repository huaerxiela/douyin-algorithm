//
// Created by jpacg on 2022/12/20.
//

#ifndef ALGORITHM_ARGUS_H
#define ALGORITHM_ARGUS_H
#include <iostream>
#include <string>

int decrypt_argus(const char *argus);
std::string encrypt_argus(const uint8_t *protobuf, uint32_t protobuf_size);


#endif //ALGORITHM_ARGUS_H
