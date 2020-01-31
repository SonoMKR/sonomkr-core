cmake_minimum_required(VERSION 3.7.0)
project(sonomkr-core VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FindPkgConfig)
include(FindBoost)

find_package(Boost REQUIRED filesystem)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})

pkg_check_modules(PKG REQUIRED alsa libzmq libconfig++ libzmqpp)

# message(${PKG_LDFLAGS} "-pthread")
# message(${PKG_CFLAGS})

file(GLOB SRC src/*.cpp)

message(" Checking RASPI variable")
if(DEFINED RASPI)
    # Best flags found so far on rpi 3. some may be useless...
    SET(RASPI_FALGS "-mfpu=neon-fp-armv8 -mfloat-abi=hard  -funsafe-math-optimizations -mvectorize-with-neon-quad -ftree-vectorize -mhard-float -latomic")
    message("  RASPI detected ! Using RASPI flags :")
    message("  ${RASPI_FALGS}")
    SET(CMAKE_CXX_FLAGS  "${RASPI_FALGS} ${CMAKE_CXX_FLAGS}")
else()
    message("  RASPI not detected")
endif()

add_compile_options(${PKG_CFLAGS})
link_libraries(${PKG_LDFLAGS} "-pthread")

add_executable(sonomkr-core ${SRC})
target_link_libraries( sonomkr-core ${Boost_LIBRARIES} )

install(TARGETS sonomkr-core RUNTIME DESTINATION /usr/local/bin/sonomkr/)
install(FILES conf/filters.conf DESTINATION /etc/sonomkr/)
install(FILES conf/sonomkr.conf DESTINATION /etc/sonomkr/)
install(FILES SonoMKR.service DESTINATION /etc/systemd/system/)

