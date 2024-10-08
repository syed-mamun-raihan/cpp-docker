cmake_minimum_required(VERSION 3.28)

project(trader)

set(CMAKE_CXX_STANDARD 26)

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
add_compile_options(-Wall -Wextra -Wpedantic -m64)

if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT $ENV{BOOST_ROOT})
endif()

if(NOT BOOST_ROOT)
    set(BOOST_ROOT "/usr")
endif()

message("BOOST_ROOT is ${BOOST_ROOT}")

find_package(Boost 1.83.0)

include_directories(${BOOST_INCLUDE_DIRS})

ADD_EXECUTABLE(trader src/TraderApp.cpp src/TradeProcessor.cpp)
