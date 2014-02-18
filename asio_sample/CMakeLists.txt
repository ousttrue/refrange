file(GLOB SRCS 
    main.cpp
    )
include_directories(
    ${CMAKE_SOURCE_DIR}/mpack/include
    $ENV{BOOST_DIR}
    )
link_directories(
    $ENV{BOOST_DIR}/lib
    )
add_executable(asio_sample ${SRCS})
