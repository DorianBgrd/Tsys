cmake_minimum_required(VERSION 3.24)
project(Tsys)

set(CMAKE_CXX_STANDARD 17)

add_definitions(/DTSYS_API_EXPORT)


find_package(Boost 1.77.0 COMPONENTS python310 REQUIRED)
find_package(Python 3.9 REQUIRED COMPONENTS Development Interpreter)


set(
        TSYS_SOURCES

        src/tsys.cpp
        src/defaultTypes.cpp
)

set(
        TSYS_INCLUDE_DIRECTORIES

        ${CMAKE_SOURCE_DIR}
        $ENV{RAPIDJSON_DIR}
        ${Boost_INCLUDE_DIRS}
        ${Python_INCLUDE_DIRS}
)


set(
        TSYS_LIBRARY_DIRECTORIES

        ${Boost_LIBRARY_DIRS}
        ${Python_LIBRARY_DIRS}
)



add_library(
        Tsys
        SHARED

        ${TSYS_SOURCES}
)


target_include_directories(
        Tsys
        PUBLIC

        ${TSYS_INCLUDE_DIRECTORIES}
)


target_link_directories(
        Tsys
        PUBLIC

        ${TSYS_LIBRARY_DIRECTORIES}
)


target_link_libraries(
        Tsys
        PUBLIC
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)






add_library(
        Tsys_Static
        STATIC

        ${TSYS_SOURCES}
)


target_include_directories(
        Tsys_Static
        PUBLIC

        ${TSYS_INCLUDE_DIRECTORIES}
)


target_link_directories(
        Tsys_Static
        PUBLIC

        ${TSYS_LIBRARY_DIRECTORIES}
)


target_link_libraries(
        Tsys_Static
        PUBLIC
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)