cmake_minimum_required(VERSION 3.7.0)
project(sonomkr-core VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FindPkgConfig)

pkg_check_modules(PKG REQUIRED alsa libzmq libconfig++ libzmqpp)

message(${PKG_LDFLAGS} "-pthread")
message(${PKG_CFLAGS})

file(GLOB SRC src/*.cpp)

add_compile_options(${PKG_CFLAGS})
link_libraries(${PKG_LDFLAGS} "-pthread")
add_executable(sonomkr-core ${SRC})
