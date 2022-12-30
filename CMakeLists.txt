cmake_minimum_required(VERSION 3.24)
project(algorithm)

set(CMAKE_CXX_STANDARD 23)

include_directories(include)

add_executable(algorithm main.cpp ladon.cpp md5.c base64.cpp simon.c sm3.c WjCryptLib_Aes.c WjCryptLib_AesCbc.c argus.cpp ByteBuf.cpp pkcs7_padding.c)
