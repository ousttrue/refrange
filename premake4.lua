-- A solution contains projects, and defines the available configurations
solution "refrange"
configurations {
    "Debug",
    "Release",
}

configuration "gmake Debug"
do
    buildoptions { "-g" }
    linkoptions { "-g" }
end

configuration "gmake"
do
    buildoptions {
        "-std=c++11",
        "-Wall",
        "-U__CYGWIN__",
    }
end

configuration "vs*"
do
    buildoptions {
        "/wd4996",
    }
end

configuration "windows"
do
    defines {
        "WIN32",
        "_WINDOWS",
    }
end

configuration "Debug"
do
    defines { "DEBUG" }
    flags { "Symbols" }
    targetdir "debug"
end

configuration "Release"
do
    defines { "NDEBUG" }
    flags { "Optimize" }
    targetdir "release"
end

configuration {}

include "loader_sample"
include "asio_sample"
include "tests"
include "gtest"

