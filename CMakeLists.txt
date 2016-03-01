cmake_minimum_required(VERSION 2.6)
project(toyunda-player)

SET(CMAKE_COLOR_MAKEFILE ON)

include("testing.cmake")

include("compil-option.cmake")

#include("doxygen.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

set(CMAKE_USE_RELATIVE_PATHS TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/src")
### Custom targets
# clang-format
# Je n'arrive pas à limiter la récursion à src et include
file(GLOB_RECURSE FILES "src/*.[ch]pp"
                        "include/*.[ch]pp")
add_custom_target(clang-format clang-format -i ${FILES}) # on ne ré-indente pas les interfaces : on ne devrait pas y toucher

# cppcheck
add_custom_target(cppcheck cppcheck --error-exitcode=1 --enable=all -I include --suppress=missingIncludeSystem ${FILES})

file(GLOB_RECURSE SRC "src/*.cpp")

#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIR})

find_package(MPV REQUIRED)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${MPV_INCLUDES_DIR})
INCLUDE_DIRECTORIES(docopt.cpp)

add_subdirectory(docopt.cpp)
add_executable(${PROJECT_NAME} ${SRC})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${MPV_LIBRARY})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} libdocopt.so)

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
