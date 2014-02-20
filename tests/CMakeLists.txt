file(GLOB SRCS 
    *
    )
include_directories(
    ${CMAKE_SOURCE_DIR}/gtest/include
    ${CMAKE_SOURCE_DIR}/refrange/include
    )
add_executable(mpack_test ${SRCS})
target_link_libraries(mpack_test gtest)
