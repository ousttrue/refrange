project "tests"
--language "C
language "C++"
--kind "StaticLib"
--kind "SharedLib"
kind "ConsoleApp"
--kind "WindowApp"

files {
    "*.cpp",
    "*.h",
}
includedirs {
    "../refrange/include",
    "../gtest/include",
}
defines {
}
links {
    "gtest",
}

