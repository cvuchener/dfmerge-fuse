cmake_minimum_required(VERSION 2.8)
project(dfmerge-fuse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# C++11 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# FUSE library
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(FUSE 2.6 REQUIRED)


aux_source_directory(src/file-formats FILEFORMATS_SOURCES)
add_library(file-formats OBJECT ${FILEFORMATS_SOURCES})

aux_source_directory(src/dfmerge-fuse DFMERGEFUSE_SOURCES)
add_executable(dfmerge-fuse ${DFMERGEFUSE_SOURCES} $<TARGET_OBJECTS:file-formats>)
set_target_properties(dfmerge-fuse PROPERTIES COMPILE_FLAGS "${FUSE_DEFINITIONS} -DFUSE_USE_VERSION=26")
target_include_directories(dfmerge-fuse PUBLIC ${FUSE_INCLUDE_DIRS})
target_link_libraries(dfmerge-fuse ${FUSE_LIBRARIES})

aux_source_directory(src/dfdiff DFDIFF_SOURCES)
add_executable(dfdiff ${DFDIFF_SOURCES} $<TARGET_OBJECTS:file-formats>)
