cmake_minimum_required(VERSION 3.15)
project(pedalcode)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ASIO SDK directory
set(ASIO_SDK_DIR "C:/ASIOSDK")

add_executable(pedalcode
    main.cpp
    ${ASIO_SDK_DIR}/common/asio.cpp
    ${ASIO_SDK_DIR}/host/asiodrivers.cpp
    ${ASIO_SDK_DIR}/host/pc/asiolist.cpp
)

target_include_directories(pedalcode PRIVATE
    ${ASIO_SDK_DIR}/common
    ${ASIO_SDK_DIR}/host
    ${ASIO_SDK_DIR}/host/pc
)

# Windows library
target_link_libraries(pedalcode
    winmm
    ole32
)