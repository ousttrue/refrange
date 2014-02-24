file(GLOB SRCS 
    *.cpp
    ../refrange/include/*.h
    ../refrange/include/refrange/*.h
    ../refrange/include/refrange/msgpack/*.h
    ../refrange/include/refrange/text/*.h
    )
#file(GLOB_RECURSE REFRANGE_HEADERS ${CMAKE_SOURCE_DIR}/refrange/include *.h)
include_directories(
    ${CMAKE_SOURCE_DIR}/gtest/include
    ${CMAKE_SOURCE_DIR}/refrange/include
    )
add_executable(mpack_test ${SRCS} ${REFRANGE_HEADERS})
target_link_libraries(mpack_test gtest)
