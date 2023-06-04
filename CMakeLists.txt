cmake_minimum_required(VERSION 3.25)
project(LeverageCalculator)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS filesystem system iostreams)
# https://cmake.org/cmake/help/latest/module/FindBoost.html

add_executable(LeverageCalculator main.cpp)

target_include_directories(LeverageCalculator PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(LeverageCalculator ${Boost_LIBRARIES})