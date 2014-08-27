project "asio_sample"
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
    "$(BOOST_DIR)",
}
defines {
}
libdirs {
    "$(BOOST_DIR)/stage/lib",
}
links {
}

