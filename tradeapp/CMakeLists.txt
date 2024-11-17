cmake_minimum_required(VERSION 3.28)

project(trader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

find_package(Boost 1.86.0)

include_directories(${BOOST_INCLUDE_DIRS})


# example library
add_library(trade_processor src/TradeProcessor.cpp)

# main code
add_executable(trader src/TraderApp.cpp)
target_link_libraries(trader PRIVATE trade_processor)

# enable FetchContent
include(FetchContent)

# declare Catch2
FetchContent_Declare(Catch2                              # name of the content
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git  # the repository
  GIT_TAG        v2.13.7                                 # the tag
  )

# make available
FetchContent_MakeAvailable(Catch2)

# testing binary
add_executable(trade_test src/test.cpp)
target_link_libraries(trade_test PRIVATE trade_processor Catch2::Catch2)

# enable testing functionality
enable_testing()

# define tests
add_test(
  NAME trade_test
  COMMAND $<TARGET_FILE:trade_test> --success
  )
