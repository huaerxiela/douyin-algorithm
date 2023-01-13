cmake_minimum_required(VERSION 3.24)
project(algorithm)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)
include_directories(crypto)

add_executable(algorithm
        main.cpp
        common.cpp
        ladon.cpp
        crypto/md5.c
        crypto/base64.cpp
        crypto/simon.c
        crypto/sm3.c
        argus.cpp
        ByteBuf.cpp
        crypto/pkcs7_padding.c
        crypto/aes.c)
