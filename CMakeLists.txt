cmake_minimum_required(VERSION 3.19)
project(bbb.agent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- load bbb_add_external() ---
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/bbb_external.cmake)

# --- output directory ---
set(C74_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# --- universal binary (macOS) ---
if(APPLE AND NOT CMAKE_OSX_ARCHITECTURES)
	set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "macOS architecture" FORCE)
endif()

# --- external dependencies (add as needed) ---
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/IXWebSocket")
	set(USE_TLS OFF CACHE BOOL "" FORCE)
	set(USE_ZLIB OFF CACHE BOOL "" FORCE)
	if(WIN32)
		set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
	endif()
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/deps/IXWebSocket)
endif()

# --- auto-discover external projects ---
MACRO(SUBDIRLIST result curdir)
	FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${curdir}/${child})
			LIST(APPEND dirlist ${child})
		ENDIF()
	ENDFOREACH()
	SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach(project_dir ${PROJECT_DIRS})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
		message("Generating: ${project_dir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
	endif()
endforeach()

set(CMAKE_SUPPRESS_REGENERATION true)

option(BBB_AGENT_BUILD_TESTS "Build JSON string round-trip test" OFF)
if(BBB_AGENT_BUILD_TESTS)
	enable_testing()
	find_package(Python3 REQUIRED COMPONENTS Interpreter)
	add_executable(bbb_agent_json_string_probe tests/json_string_probe.cpp)
	target_include_directories(bbb_agent_json_string_probe PRIVATE source/bbb)
	add_test(NAME bbb_agent_json_strings COMMAND ${Python3_EXECUTABLE}
		${CMAKE_CURRENT_SOURCE_DIR}/tests/check_json_string.py $<TARGET_FILE:bbb_agent_json_string_probe>)
endif()
