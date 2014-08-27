project "loader_sample"
--language "C
language "C++"
--kind "StaticLib"
--kind "SharedLib"
kind "ConsoleApp"
--kind "WindowApp"

files {
    "main.cpp",
}
includedirs {
    "../refrange/include",
}
defines {
}
links {
}

