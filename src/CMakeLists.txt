FIND_PACKAGE(LibEV REQUIRED)
FIND_PACKAGE (Threads REQUIRED)

INCLUDE_DIRECTORIES (${SPEEDO_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES (${SPEEDO_SOURCE_DIR}/src/io)
INCLUDE_DIRECTORIES (${SPEEDO_SOURCE_DIR}/src/log)
INCLUDE_DIRECTORIES (${SPEEDO_SOURCE_DIR}/src/messagequeue)

ADD_SUBDIRECTORY (io)
ADD_SUBDIRECTORY (protocol)

LINK_LIBRARIES (pthread)
LINK_LIBRARIES (ev)



# LINK_DIRECTORIES (${SPEEDO_BINARY_DIR}/src/io) 

ADD_EXECUTABLE (speedo main.cpp)


TARGET_LINK_LIBRARIES (speedo io ev protocol ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

IF(ENABLE_GPERF_TOOLS MATCHES "ON")
    TARGET_LINK_LIBRARIES(speedo profiler)
    TARGET_LINK_LIBRARIES(speedo tcmalloc)
ENDIF(ENABLE_GPERF_TOOLS MATCHES "ON")

