cmake_minimum_required(VERSION 3.25)
project(Tsys)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DTSYS_API_EXPORT)


find_package(Boost 1.77.0 COMPONENTS python310 REQUIRED)
find_package(Python 3.9 REQUIRED COMPONENTS Development Interpreter)


add_library(
        Tsys
        SHARED
        src/tsys.cpp
        src/defaultTypes.cpp
)


target_include_directories(
        Tsys
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)


target_link_directories(
        Tsys
        PUBLIC
        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)


target_link_libraries(
        Tsys
        PUBLIC
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)