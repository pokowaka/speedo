# The name of our project is "HELLO". CMakeLists files in this project can 
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and 
# to the root binary directory of the project as ${HELLO_BINARY_DIR}. 
CMAKE_MINIMUM_REQUIRED (VERSION 2.8) 
PROJECT (PROCTOR) 

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
SET(PACKAGE_VERSION 2.0.0)

INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckVariableExists)
INCLUDE(CheckTypeSize)
INCLUDE(CheckLibraryExists)
INCLUDE(CMakeDetermineCCompiler)
INCLUDE(FindThreads)
INCLUDE(FindPkgConfig)


OPTION(DEBUG_MODE          "Enable debug output [default: ON]"      ON)
OPTION(ENABLE_GPERF_TOOLS  "Enable google perftools [default: OFF]" OFF)


# Valgrind  & coverage thingies.
IF(DEBUG_MODE MATCHES "ON")
  MESSAGE ("Setting debug flags")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
  INCLUDE_DIRECTORIES ("/usr/local/include/valgrind")
  IF (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    LINK_LIBRARIES(gcov)
  ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-optimize-sibling-calls")
  ENDIF ()
ELSE()
  MESSAGE("Setting release flags")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
ENDIF(DEBUG_MODE MATCHES "ON")


ADD_DEFINITIONS(-DHAVE_INET6) # Necessary for getting the right size of some

IF (COVERAGE AND "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET (CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")
  SET (CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fprofile-arcs -ftest-coverage")
  ADD_DEFINITIONS(-fprofile-arcs -ftest-coverage)
  LINK_LIBRARIES(gcov)
ENDIF ()



# add subdirectory at the end so we inherit all values SET up here.
ADD_SUBDIRECTORY (src)

################################
# Testing
################################
IF (test)
  # This adds another subdirectory, which has 'project(gtest)'.
  ADD_SUBDIRECTORY(lib/gtest-1.6.0)

  ENABLE_TESTING()

  # Include the gtest library. gtest_SOURCE_DIR is available due to
  # 'project(gtest)' above.
  INCLUDE_DIRECTORIES(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

  ##############
  # Unit Tests
  ##############
  #add_executable(runUnitTests test_project1.cpp)

  # Standard linking to gtest stuff.
  #target_link_libraries(runUnitTests gtest gtest_main)

  # Extra linking for the project.
  #target_link_libraries(runUnitTests project1_lib)

  # This is so you can do 'make test' to see all your tests run, instead of
  # manually running the executable runUnitTests to see those specific tests.
  #add_test(NAME that-test-I-made COMMAND runUnitTests)

  # You can also omit NAME and COMMAND. The second argument could be some other
  # test executable.
  #add_test(that-other-test-I-made runUnitTests)
ENDIF()
