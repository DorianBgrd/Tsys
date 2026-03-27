cmake_minimum_required(VERSION 3.24)

project(TypeSystem)

set(CMAKE_CXX_STANDARD 17)

include(Third-parties.cmake)

add_definitions(/DTSYS_API_EXPORT)

set(CMAKE_DEBUG_POSTFIX -dbg)

set(
        TSYS_SOURCES

        src/tsys.cpp
        src/defaultTypes.cpp
)

set(
        TSYS_INCLUDES

        include/defaultTypes.h
        include/tsys.h
)


add_library(
        Tsys
        SHARED

        ${TSYS_SOURCES}
)

message(STATUS "Chemin des includes : ${CMAKE_CURRENT_SOURCE_DIR}/releases")


target_include_directories(
        Tsys

        INTERFACE
        $<INSTALL_INTERFACE:include>

        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
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
        ${Python_LINK_LIBRARIES}
)


set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/releases/Tsys-Python${Python_USE_VERSION})


install(TARGETS Tsys
        EXPORT Tsys
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib
        INCLUDES DESTINATION include
)


install(
        EXPORT Tsys
        FILE TsysTargets.cmake
        NAMESPACE Tsys::
        DESTINATION cmake
)


# Generate and install configuration file.
include(CMakePackageConfigHelpers)


configure_package_config_file(
        ${CMAKE_SOURCE_DIR}/cmake/TsysConfig.cmake.in
        TsysConfig.cmake
        INSTALL_DESTINATION cmake
)

install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/TsysConfig.cmake
        DESTINATION cmake
)

install(
        DIRECTORY
        "include/Tsys"
        DESTINATION
        include
)