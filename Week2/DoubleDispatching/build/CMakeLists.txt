cmake_minimum_required (VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)

set (PROJECT 
	DoubleDispatching)

set (HEADERS
	../geo2d.h
	../game_object.h)

set (SOURCES 
	../geo2d.cpp
	../collide.cpp)

set (TESTS_SOURCES)

project (${PROJECT})

source_group ("Header Files" FILES ${HEADERS})
source_group ("Source Files" FILES ${SOURCES})

include_directories(../../../include)

if (TESTS_SOURCES)
	set (Boost_USE_STATIC_LIBS ON)
	set (Boost_USE_MULTITHREADED ON)
	find_package (Boost
		COMPONENTS
			unit_test_framework
		REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
	set (TEST test_${PROJECT})
	add_executable (${TEST} ${TESTS_SOURCES})
	target_link_libraries (${TEST} ${PROJECT} ${Boost_LIBRARIES})
	enable_testing ()
	add_test (${TEST} ${TEST})
else ()
    add_executable (${PROJECT} ${HEADERS} ${SOURCES})
endif()
set_target_properties(${PROJECT} PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
