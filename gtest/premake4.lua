project "gtest"
--language "C
language "C++"
kind "StaticLib"
--kind "SharedLib"
--kind "ConsoleApp"
--kind "WindowApp"

files {
    "include/*",
    "src/*",
}
includedirs {
    "include",
    ".",
}
defines {
}
links {
}

