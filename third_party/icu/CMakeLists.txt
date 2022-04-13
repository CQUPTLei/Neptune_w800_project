cmake_minimum_required(VERSION 3.16.5)

set(CMAKE_CXX_STANDARD 11)

project(icu)

set(LibIcuSrc "${PROJECT_SOURCE_DIR}/icu4c/source/common")

include_directories(${LibIcuSrc})

MESSAGE("ICU_INCLUDE_DIRS: " ${LibIcuSrc})

add_library(icu STATIC
    ${LibIcuSrc}/cmemory.cpp
    ${LibIcuSrc}/cstring.cpp
    ${LibIcuSrc}/ubidi.cpp
    ${LibIcuSrc}/ubidiln.cpp
    ${LibIcuSrc}/ubidiwrt.cpp
    ${LibIcuSrc}/ubidi_props.cpp
    ${LibIcuSrc}/uchar.cpp
    ${LibIcuSrc}/udataswp.cpp
    ${LibIcuSrc}/uinvchar.cpp
    ${LibIcuSrc}/ustring.cpp
    ${LibIcuSrc}/utf_impl.cpp
    ${LibIcuSrc}/utrie2.cpp
    )

target_include_directories(icu PUBLIC ${LibIcuSrc})
