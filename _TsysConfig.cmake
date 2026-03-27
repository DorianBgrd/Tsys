
if (DEFINED Python_USE_VERSION)
    set(BUILD_SUFFIX "-python${Python_USE_VERSION}")
endif()


set(
        TSYS_LIBRARY_DIR
        "${CMAKE_CURRENT_LIST_DIR}/cmake-build-${CMAKE_BUILD_TYPE}${BUILD_SUFFIX}"
)


set(
        TSYS_LIBRARIES

        Tsys
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)


set(
        TSYS_STATIC_LIBRARIES

        Tsys_Static
        ${Boost_LIBRARIES}
        Python::Python
        Python::Module
)


set(
        TSYS_INCLUDE_DIRS
        "${CMAKE_CURRENT_LIST_DIR}/include"
)