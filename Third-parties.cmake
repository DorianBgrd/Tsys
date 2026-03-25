set(Boost_USE_STATIC_LIBS OFF)
set(Boost_DIR $ENV{BOOST_DIR})
set(Python_ROOT_DIR $ENV{Python_DIR})

find_package(Python REQUIRED COMPONENTS Development Interpreter)
find_package(Boost 1.82.0 COMPONENTS python${Python_USE_VERSION} REQUIRED HINTS $ENV{BOOST_ROOT})

# Due to boost build not using python debug by default, and
# sometimes not linking it correctly when built from source
# (linking to both python libs) it is disabled by default and
# only release version of python is linked.
set(Python_LINK_LIBRARIES ${Python_LIBRARIES})

if (DEFINED ENV{Python_DEBUG})
    message("Python_DEBUG variable is set to $ENV{Python_DEBUG} (From environment)")
    if ($ENV{Python_DEBUG})
        set(Python_LINK_LIBRARIES ${Python_LIBRARIES})
    else()
        set(Python_LINK_LIBRARIES ${Python_LIBRARY_RELEASE})
    endif()
endif()

message("Python link libraries : ${Python_LINK_LIBRARIES}")