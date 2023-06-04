cmake_minimum_required(VERSION 3.25)
project(LeverageCalculator)

set(CMAKE_CXX_STANDARD 20)

add_executable(LeverageCalculator main.cpp)
include_directories(/opt/homebrew/include/)
target_link_libraries(LeverageCalculator /opt/homebrew/lib/libboost_iostreams.a /opt/homebrew/lib/libboost_system.a /opt/homebrew/lib/libboost_filesystem.a)