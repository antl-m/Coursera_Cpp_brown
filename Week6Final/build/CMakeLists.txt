cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set(PROJECT
        TransportDirectory)

set(HEADERS
        ../tests/tests.h
        ../PartA/DataBase.h
        ../PartA/ModifyQuery.h
        ../PartA/ReadQuery.h
        ../PartA/Stop.h
        ../PartA/Bus.h ../PartA/other.h)

set(SOURCES
        ../PartA/MainPartA.cpp
        ../tests/TestsPartA.cpp
        ../PartA/DataBase.cpp
        ../PartA/ModifyQuery.cpp
        ../PartA/ReadQuery.cpp
        ../PartA/Stop.cpp
        ../PartA/Bus.cpp)

project(${PROJECT})

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

include_directories(../../include
                    ../PartA
                    ../tests)

add_executable(${PROJECT} ${HEADERS} ${SOURCES})
set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
