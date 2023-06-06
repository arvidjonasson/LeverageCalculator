cmake_minimum_required(VERSION 3.25)
project(LeverageCalculator)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED COMPONENTS filesystem system iostreams)

add_executable(LeverageCalculator)
add_subdirectory(src)

target_include_directories(LeverageCalculator PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(LeverageCalculator 
    PRIVATE
    Boost::filesystem
    Boost::system
    Boost::iostreams
)

if(MSVC)
    target_compile_options(LeverageCalculator PRIVATE /W4 /WX)
else()
    target_compile_options(LeverageCalculator PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()