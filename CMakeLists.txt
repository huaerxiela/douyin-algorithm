cmake_minimum_required(VERSION 3.24)
project(algorithm)

set(CMAKE_CXX_STANDARD 23)

find_package(Protobuf REQUIRED)


include_directories(include)
include_directories(crypto)

add_executable(algorithm main.cpp ladon.cpp md5.c base64.cpp simon.c sm3.c argus.cpp ByteBuf.cpp pkcs7_padding.c crypto/aes.c)

#target_link_libraries(algorithm
#        Protobuf::libprotobuf
#        )
