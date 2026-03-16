cmake_minimum_required(VERSION 3.24)
project(Tsys)

set(CMAKE_CXX_STANDARD 17)

include(Third-parties.cmake)

add_definitions(/DTSYS_API_EXPORT)

set(
        TSYS_SOURCES

        src/tsys.cpp
        src/defaultTypes.cpp
)

set(
        TSYS_INCLUDES
        include/tsys.h
        include/defaultTypes.h
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


set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/install/Tsys)


install(TARGETS Tsys DESTINATION bin
        CONFIGURATIONS Debug Release)

install(FILES ${TSYS_INCLUDES}
        DESTINATION include)

export(TARGETS Tsys FILE ${CMAKE_SOURCE_DIR}/install/Tsys/FindTSys.cmake)