if(compil-option_included)
    return()
endif(compil-option_included)
set(compil-option_included true)

set(COMPIL_OPTION_INCLUDED 1)

include(CheckCXXCompilerFlag)
function(add_flag var flag)
    check_cxx_compiler_flag(${flag} ${var}${flag})
    if(${var}${flag})
        set(${var} "${${var}} ${flag}" PARENT_SCOPE)
    endif()
endfunction(add_flag)

### Config des options de compilation
# Debug/Release :
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang"
 OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using Clang or gcc
    add_flag(CMAKE_CXX_FLAGS                -std=c++1z)
    add_flag(CMAKE_CXX_FLAGS                -Wall)
    add_flag(CMAKE_CXX_FLAGS                -Wextra)
    add_flag(CMAKE_CXX_FLAGS                -pedantic)
    add_flag(CMAKE_CXX_FLAGS                -Wshadow)
    add_flag(CMAKE_CXX_FLAGS                -pipe)
    add_flag(CMAKE_CXX_FLAGS                -fdiagnostics-color=auto)
    add_flag(CMAKE_CXX_FLAGS_DEBUG          -g)
    add_flag(CMAKE_CXX_FLAGS_DEBUG          -Og)
    add_flag(CMAKE_CXX_FLAGS_DEBUG          -fsanitize=address)
    add_flag(CMAKE_CXX_FLAGS_RELEASE        -O2)
    add_flag(CMAKE_CXX_FLAGS_RELEASE        -DNDEBUG)
    add_flag(CMAKE_CXX_FLAGS_RELEASE        -flto)
    add_flag(CMAKE_CXX_FLAGS_RELWITHDEBINFO -g)
    add_flag(CMAKE_CXX_FLAGS_RELWITHDEBINFO -O2)
    add_flag(CMAKE_CXX_FLAGS_RELWITHDEBINFO -flto)

    add_flag(CMAKE_EXE_LINKER_FLAGS_RELEASE -s)
    add_flag(CMAKE_EXE_LINKER_FLAGS_RELEASE -flto)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(EXEC_NAME "${EXEC_NAME}_debug")
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

# TODO : voir si ça marche
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)

# Cible pour clean
# TODO : ajouter d'autres diretives de clean
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES *.o)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES *.so)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES *.a)
