file(GLOB SRCS 
    main.cpp
    )
include_directories(
    ${CMAKE_SOURCE_DIR}/refrange/include
    $ENV{BOOST_DIR}
    $ENV{BOOST_DIR}/include/boost-1_55
    )
link_directories(
    $ENV{BOOST_DIR}/lib
    )
add_executable(loader_sample ${SRCS})
