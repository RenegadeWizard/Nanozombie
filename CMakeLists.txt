cmake_minimum_required(VERSION 3.16)
project(Nanozombie)

find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

add_executable(nanozombie
        src/nano_struct.h
        nanozombie.cpp src/Voyager.cpp src/Voyager.h src/nano_struct.cpp src/Logger.cpp src/Logger.h)

target_link_libraries(nanozombie ${MPI_LIBRARIES})

#include_directories(.)
include_directories(${MPI_INCLUDE_PATH})
if (MPI_COMPILE_FLAGS)
    set_target_properties(nanozombie PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif ()
if (MPI_LINK_FLAGS)
    set_target_properties(nanozombie PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
endif ()

