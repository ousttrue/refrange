file(GLOB SRCS 
    src/*
    )
file(GLOB HEADERS
    inculde/*
    )
include_directories(
    ${CMAKE_SOURCE_DIR}/gtest
    ${CMAKE_SOURCE_DIR}/gtest/include
    )
add_library(gtest STATIC ${SRCS} ${HEADERS})

