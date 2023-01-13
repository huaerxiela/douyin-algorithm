//
// Created by jpacg on 2023/1/13.
//

#ifndef ALGORITHM_COMMON_H
#define ALGORITHM_COMMON_H

uint32_t padding_size(uint32_t size);
std::string md5bytes(uint8_t *data, uint32_t size);

template<class T> void set_type_data(const uint8_t *ptr, int index, T data);
template<class T> T get_type_data(const uint8_t *ptr, int index);

#endif //ALGORITHM_COMMON_H
