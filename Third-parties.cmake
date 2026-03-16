set(Boost_USE_STATIC_LIBS OFF)
set(Boost_DIR $ENV{BOOST_DIR})
set(Python_ROOT_DIR $ENV{Python_DIR})

find_package(Python REQUIRED COMPONENTS Development Interpreter)
find_package(Boost 1.82.0 COMPONENTS python${Python_USE_VERSION} REQUIRED HINTS $ENV{BOOST_ROOT})
