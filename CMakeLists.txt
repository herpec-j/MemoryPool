cmake_minimum_required(VERSION 2.6)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

project(MemoryPool)

if (MSVC)

else()
	include(CheckCXXCompilerFlag)
	CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
	CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if (COMPILER_SUPPORTS_CXX11)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	elseif (COMPILER_SUPPORTS_CXX0X)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	else ()
		message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
	endif ()
endif ()

set(MemoryPool_HDRS
	${PROJECT_SOURCE_DIR}/MemoryPool/Includes/MemoryPool/BufferPool.hpp
	${PROJECT_SOURCE_DIR}/MemoryPool/Includes/MemoryPool/ObjectPool.hpp
	${PROJECT_SOURCE_DIR}/MemoryPool/Includes/MemoryPool/ThreadSafeObjectPool.hpp)

source_group(Includes FILES ${MemoryPool_HDRS})

set(MemoryPool_Private_HDRS
	${PROJECT_SOURCE_DIR}/MemoryPool/Includes/MemoryPool/Private/SpinLock.hpp)

source_group(Includes\\Private FILES ${MemoryPool_Private_HDRS})

set(HDRS
	${MemoryPool_HDRS}
	${MemoryPool_Private_HDRS})
	
set(SRCS
	${PROJECT_SOURCE_DIR}/MemoryPool/Sources/Test.cpp)

source_group(Sources FILES ${SRCS})

include_directories("$(PROJECT_SOURCE_DIR)/MemoryPool/Includes")

add_executable(MemoryPoolTest ${SRCS} ${HDRS})

enable_testing()

add_test(MemoryPoolUnitTests MemoryPoolTest)

set_tests_properties(MemoryPoolUnitTests
	PROPERTIES PASS_REGULAR_EXPRESSION "Success!")